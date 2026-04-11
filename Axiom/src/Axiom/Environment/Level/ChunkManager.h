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

		void SendInitialChunks(const Ref<Connection> &connection, double playerX, double playerZ);
		void OnPlayerMove(const Ref<Connection> &connection, double playerX, double playerZ);
		void RemovePlayer(ConnectionId connectionId);

		/**
		 * Promote a skeletal chunk to Full and re-send it to
		 * every player that currently has it loaded. Called by
		 * the hot-radius sweep when a player approaches a cold
		 * chunk; the regeneration and the re-send happen on the
		 * generator worker pool, off the tick thread.
		 */
		void UpgradeChunk(ChunkPosition chunkPosition);

		/**
		 * Radius, in chunks, inside which chunks are generated
		 * at Full tier. Chunks outside this radius (but inside
		 * the player's total view distance) are generated at
		 * Skeletal tier and upgraded lazily.
		 */
		int HotRadius() const { return m_HotRadius; }
		void SetHotRadius(const int radius) { m_HotRadius = radius; }

		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const;

		ChunkGenerator& Generator() { return *m_Generator; }
		const ChunkGenerator& Generator() const { return *m_Generator; }

		int ViewDistance() const { return m_ViewDistance; }

		void SetPlayerViewDistance(Ref<Connection> connection, int requestedDistance);

		std::unordered_map<ConnectionId, int> SnapshotViewDistances() const;

		using ChunkSentCallback = std::function<void(ChunkPosition chunkPosition)>;
		void SetChunkSentCallback(ChunkSentCallback callback) { m_ChunkSentCallback = std::move(callback); }

		using ChunkUnloadCallback = std::function<void(ChunkPosition chunkPosition)>;
		void SetChunkUnloadCallback(ChunkUnloadCallback callback) { m_ChunkUnloadCallback = std::move(callback); }

		void MarkChunkDirty(int32_t chunkX, int32_t chunkZ);
		void SaveAllDirtyChunks();

	private:
		struct PlayerChunkState {
			Ref<Connection> connection;
			int32_t lastChunkX = 0;
			int32_t lastChunkZ = 0;
			int effectiveViewDistance = 0;
			std::unordered_set<ChunkPosition> loadedChunks;
		};

		void QueueChunksInRadius(Ref<Connection> connection, ChunkPosition centerPosition);

		// Caller must hold m_StateMutex. Mutates state.loadedChunks and
		// returns positions for UnloadChunk to dispatch after releasing
		// the lock — keeps blocking network I/O off the lock.
		std::vector<ChunkPosition> CollectDistantChunks(const ChunkPosition& centerPosition, PlayerChunkState& state);

		void SendChunk(Ref<Connection> connection, ChunkPosition chunkPosition, ChunkTier tier);
		void UnloadChunk(Ref<Connection> connection, ChunkPosition chunkPosition);

		/**
		 * Drop the wire-byte cache entry for a chunk when no
		 * player has it loaded anymore. Called after UnloadChunk
		 * and from RemovePlayer; bounds m_ChunkDataCache so it
		 * can't grow without limit over a long-running session.
		 */
		void EvictChunkDataIfOrphaned(ChunkPosition chunkPosition);

		/**
		 * Returns the generation tier for a chunk at chunk-space
		 * Chebyshev distance `distance` from a player whose hot
		 * radius is `m_HotRadius`. Chunks inside the hot radius
		 * are Full; everything else is Skeletal.
		 */
		ChunkTier TierForDistance(int distance) const {
			return distance <= m_HotRadius ? ChunkTier::Full : ChunkTier::Skeletal;
		}

		void WorkerLoop();
		void SubmitTask(std::function<void()> task);

		static int32_t BlockToChunk(double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

		Scope<ChunkGenerator> m_Generator;
		int m_ViewDistance;
		int m_HotRadius = 8;

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

		/**
		 * Wire-bytes cache keyed by chunk position. Stores the
		 * most recent `ChunkData` produced for a chunk along
		 * with the tier it was generated at, so SendChunk can
		 * skip re-running the generator if a request for the
		 * same-or-lower tier comes in while the entry is still
		 * live, and UpgradeChunk can cheaply detect whether
		 * promotion is needed.
		 */
		struct CachedChunkData {
			ChunkData data;
			ChunkTier tier;
		};

		mutable std::mutex m_ChunkDataCacheMutex;
		std::unordered_map<int64_t, CachedChunkData> m_ChunkDataCache;

		static int64_t ChunkKey(int32_t chunkX, int32_t chunkZ) {
			return (static_cast<int64_t>(chunkX) << 32) | (static_cast<uint32_t>(chunkZ));
		}

		void CacheChunkBlocks(int32_t chunkX, int32_t chunkZ);
	};

}
