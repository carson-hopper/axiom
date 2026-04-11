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

		void SendInitialChunks(Ref<Connection> connection, double playerX, double playerZ);
		void OnPlayerMove(const Ref<Connection> &connection, double playerX, double playerZ);
		void RemovePlayer(ConnectionId connectionId);

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

		void SendChunk(Ref<Connection> connection, ChunkPosition chunkPosition);
		void UnloadChunk(Ref<Connection> connection, ChunkPosition chunkPosition);

		void WorkerLoop();
		void SubmitTask(std::function<void()> task);

		static int32_t BlockToChunk(double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

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
