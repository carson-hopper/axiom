#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Chunk.h"
#include "Generator/ChunkGenerator.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Axiom {

	struct ChunkPosition {
		int32_t x;
		int32_t z;

		bool operator==(const ChunkPosition& other) const {
			return x == other.x && z == other.z;
		}

		bool operator<(const ChunkPosition& other) const {
			return x < other.x || (x == other.x && z < other.z);
		}
	};

}

/**
 * Specialisation so ChunkPosition can be used as a key
 * in std::unordered_set / std::unordered_map. Packs
 * (x, z) into a single uint64_t and feeds that through
 * std::hash<uint64_t>, which on modern stdlibs is
 * already a good bit-mixer for hash-indexed buckets.
 * Cast to uint32_t first to avoid sign-extension on
 * negative chunk coordinates.
 */
template<>
struct std::hash<Axiom::ChunkPosition> {
	std::size_t operator()(const Axiom::ChunkPosition& position) const noexcept {
		const std::uint64_t packed =
			(static_cast<std::uint64_t>(static_cast<std::uint32_t>(position.x)) << 32)
			| static_cast<std::uint64_t>(static_cast<std::uint32_t>(position.z));
		return std::hash<std::uint64_t>{}(packed);
	}
};

namespace Axiom {

	class ChunkManager {
	public:
		explicit ChunkManager(Scope<ChunkGenerator> generator, int viewDistance = 10);
		~ChunkManager();

		ChunkManager(const ChunkManager&) = delete;
		ChunkManager& operator=(const ChunkManager&) = delete;

		void SendInitialChunks(Ref<Connection> connection, double playerX, double playerZ);
		void OnPlayerMove(const Ref<Connection> &connection, double playerX, double playerZ);
		void RemovePlayer(ConnectionId connectionId);

		/**
		 * Query the block state at world coordinates.
		 * Returns 0 (air) for unloaded or out-of-range positions.
		 */
		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const;

		ChunkGenerator& Generator() { return *m_Generator; }
		const ChunkGenerator& Generator() const { return *m_Generator; }

		/**
		 * Server-wide maximum render distance. Individual
		 * players may render at a SMALLER radius via
		 * `SetPlayerViewDistance`, but never larger.
		 */
		int ViewDistance() const { return m_ViewDistance; }

		/**
		 * Override the effective render distance for a
		 * single player. `requestedDistance` is clamped to
		 * `[2, ViewDistance()]` so a misbehaving client
		 * can't force the server to generate more chunks
		 * than the server-side maximum. If the new radius
		 * is smaller than the old one, any now-out-of-range
		 * chunks are immediately unloaded; if it's larger,
		 * the new outer rings are queued for generation.
		 * Safe to call for a connection that has no chunk
		 * state yet — the value is stored on the connection
		 * and picked up when `SendInitialChunks` runs.
		 */
		void SetPlayerViewDistance(Ref<Connection> connection, int requestedDistance);

		/**
		 * Snapshot every tracked player's effective view
		 * distance into a single map. Taken under one
		 * acquisition of `m_StateMutex`; the caller can
		 * iterate the returned map freely without lock
		 * contention. Used by `NetworkServer::BroadcastPacketNearby`
		 * to filter observers by `min(source, observer)`
		 * view distance in one pass.
		 *
		 * Players that have not yet entered the Play state
		 * are not represented in the result.
		 */
		std::unordered_map<ConnectionId, int> SnapshotViewDistances() const;

		using ChunkSentCallback = std::function<void(ChunkPosition chunkPosition)>;
		void SetChunkSentCallback(ChunkSentCallback callback) { m_ChunkSentCallback = std::move(callback); }

		using ChunkUnloadCallback = std::function<void(ChunkPosition chunkPosition)>;
		void SetChunkUnloadCallback(ChunkUnloadCallback callback) { m_ChunkUnloadCallback = std::move(callback); }

		/** Mark a chunk as dirty (modified by player). */
		void MarkChunkDirty(int32_t chunkX, int32_t chunkZ);

		/** Save all dirty chunks. Called on shutdown. */
		void SaveAllDirtyChunks();

	private:
		struct PlayerChunkState {
			int32_t lastChunkX = 0;
			int32_t lastChunkZ = 0;
			/**
			 * Per-player effective render distance in
			 * chunks. Seeded from the client's
			 * `ClientInformation` request (clamped to
			 * the server maximum) when the player
			 * enters Play state, then updated by
			 * `SetPlayerViewDistance` for mid-game
			 * changes. Always in `[2, ViewDistance()]`.
			 */
			int effectiveViewDistance = 0;
			std::unordered_set<ChunkPosition> loadedChunks;
		};

		void QueueChunksInRadius(Ref<Connection> connection, ChunkPosition centerPosition);

		/**
		 * Pure under-lock helper: compute the set of
		 * chunk positions the player has loaded that
		 * are now outside the view radius, erase them
		 * from `state.loadedChunks`, and return them
		 * to the caller so that `UnloadChunk` can be
		 * invoked *without* the state mutex held.
		 * This keeps blocking network I/O off the lock.
		 */
		std::vector<ChunkPosition> CollectDistantChunks(const ChunkPosition& centerPosition, PlayerChunkState& state);

		void SendChunk(Ref<Connection> connection, ChunkPosition chunkPosition);
		void UnloadChunk(Ref<Connection> connection, ChunkPosition chunkPosition);

		void WorkerLoop();
		void SubmitTask(std::function<void()> task);

		static int32_t BlockToChunk(double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

		/**
		 * Sole owner of this manager's generator. Per
		 * STYLE.md §4.3, `Scope<T>` is the default for
		 * owning pointers — `Ref<T>` would imply shared
		 * ownership, which no other subsystem takes
		 * here. Observers go through `Generator()`
		 * which returns a plain reference.
		 */
		Scope<ChunkGenerator> m_Generator;
		int m_ViewDistance;

		mutable std::mutex m_StateMutex;
		std::unordered_map<ConnectionId, PlayerChunkState> m_PlayerStates;

		std::vector<std::thread> m_Workers;
		std::queue<std::function<void()>> m_TaskQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_QueueCondition;
		std::atomic<bool> m_Stopping = false;
		ChunkSentCallback m_ChunkSentCallback;
		ChunkUnloadCallback m_ChunkUnloadCallback;
		std::unordered_set<int64_t> m_DirtyChunks;
		std::mutex m_DirtyMutex;

		mutable std::mutex m_ChunkCacheMutex;
		std::unordered_map<int64_t, Ref<Chunk>> m_ChunkCache;

		static int64_t ChunkKey(int32_t chunkX, int32_t chunkZ) {
			return (static_cast<int64_t>(chunkX) << 32) | (static_cast<uint32_t>(chunkZ));
		}

		void CacheChunkBlocks(int32_t chunkX, int32_t chunkZ);
	};

}
