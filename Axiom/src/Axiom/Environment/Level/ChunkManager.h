#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Chunk.h"
#include "Generator/ChunkGenerator.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <set>
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

	class ChunkManager {
	public:
		explicit ChunkManager(Ref<ChunkGenerator> generator, int viewDistance = 10);
		~ChunkManager();

		ChunkManager(const ChunkManager&) = delete;
		ChunkManager& operator=(const ChunkManager&) = delete;

		void SendInitialChunks(Ref<Connection> connection, double playerX, double playerZ);
		void OnPlayerMove(Ref<Connection> connection, double playerX, double playerZ);
		void RemovePlayer(ConnectionId connectionId);

		/**
		 * Query the block state at world coordinates.
		 * Returns 0 (air) for unloaded or out-of-range positions.
		 */
		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const;

		ChunkGenerator& Generator() { return *m_Generator; }
		const ChunkGenerator& Generator() const { return *m_Generator; }
		int ViewDistance() const { return m_ViewDistance; }

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
			std::set<ChunkPosition> loadedChunks;
		};

		void QueueChunksInRadius(Ref<Connection> connection, ChunkPosition centerPosition);
		void UnloadDistantChunks(Ref<Connection> connection, ChunkPosition centerPosition, PlayerChunkState& state);
		void SendChunk(Ref<Connection> connection, ChunkPosition chunkPosition);
		void UnloadChunk(Ref<Connection> connection, ChunkPosition chunkPosition);

		void WorkerLoop();
		void SubmitTask(std::function<void()> task);

		static int32_t BlockToChunk(double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

		Ref<ChunkGenerator> m_Generator;
		int m_ViewDistance;

		std::mutex m_StateMutex;
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
