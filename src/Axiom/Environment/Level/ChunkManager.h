#pragma once

#include "Axiom/Core/Base.h"
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

	/**
	 * Manages chunk loading and unloading per connection.
	 * Generates chunks asynchronously on worker threads and streams
	 * them to clients as they complete.
	 */
	class ChunkManager {
	public:
		explicit ChunkManager(Ref<ChunkGenerator> generator, int viewDistance = 10);
		~ChunkManager();

		ChunkManager(const ChunkManager&) = delete;
		ChunkManager& operator=(const ChunkManager&) = delete;

		void SendInitialChunks(Ref<Connection> connection, double playerX, double playerZ);
		void OnPlayerMove(Ref<Connection> connection, double playerX, double playerZ);
		void RemovePlayer(Connection* connection);

		ChunkGenerator& Generator() const { return *m_Generator; }
		int ViewDistance() const { return m_ViewDistance; }

		using ChunkSentCallback = std::function<void(int32_t chunkX, int32_t chunkZ)>;
		void SetChunkSentCallback(ChunkSentCallback callback) { m_ChunkSentCallback = std::move(callback); }

	private:
		struct PlayerChunkState {
			int32_t lastChunkX = 0;
			int32_t lastChunkZ = 0;
			std::set<ChunkPosition> loadedChunks;
		};

		void QueueChunksInRadius(Ref<Connection> connection, int32_t centerX, int32_t centerZ);
		void UnloadDistantChunks(Ref<Connection> connection, int32_t centerX, int32_t centerZ,
			PlayerChunkState& state);
		void SendChunk(const Ref<Connection>& connection, int32_t chunkX, int32_t chunkZ) const;
		void UnloadChunk(const Ref<Connection>& connection, int32_t chunkX, int32_t chunkZ);

		// Worker thread pool
		void WorkerLoop();
		void SubmitTask(std::function<void()> task);

		static int32_t BlockToChunk(double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

		Ref<ChunkGenerator> m_Generator;
		int m_ViewDistance;

		std::mutex m_StateMutex;
		std::unordered_map<Connection*, PlayerChunkState> m_PlayerStates;

		// Thread pool
		std::vector<std::thread> m_Workers;
		std::queue<std::function<void()>> m_TaskQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_QueueCondition;
		std::atomic<bool> m_Stopping = false;
		ChunkSentCallback m_ChunkSentCallback;
	};

}
