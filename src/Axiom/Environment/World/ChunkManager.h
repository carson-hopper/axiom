#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/World/ChunkGenerator.h"
#include "Axiom/Network/Connection.h"

#include <mutex>
#include <set>
#include <unordered_map>

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
	 * Tracks which chunks each player has loaded, sends new chunks
	 * when they move, and unloads chunks outside view distance.
	 */
	class ChunkManager {
	public:
		explicit ChunkManager(Ref<ChunkGenerator> generator, const int viewDistance = 10)
			: m_Generator(std::move(generator))
			, m_ViewDistance(viewDistance) {}

		/**
		 * Send initial chunks around spawn for a newly joined player.
		 */
		void SendInitialChunks(const Ref<Connection> &connection, double playerX, double playerZ);

		/**
		 * Called when a player moves. Sends new chunks and unloads old ones
		 * if the player crossed a chunk border.
		 */
		void OnPlayerMove(const Ref<Connection> &connection, double playerX, double playerZ);

		/**
		 * Remove tracking data for a disconnected player.
		 */
		void RemovePlayer(Connection* connection);

		ChunkGenerator& Generator() const { return *m_Generator; }
		int ViewDistance() const { return m_ViewDistance; }

	private:
		struct PlayerChunkState {
			int32_t lastChunkX = 0;
			int32_t lastChunkZ = 0;
			std::set<ChunkPosition> loadedChunks;
		};

		void SendChunksInRadius(const Ref<Connection> &connection, int32_t centerX, int32_t centerZ,
			PlayerChunkState& state);
		void UnloadDistantChunks(const Ref<Connection> &connection, int32_t centerX, int32_t centerZ,
			PlayerChunkState& state);
		void SendChunk(const Ref<Connection> &connection, int32_t chunkX, int32_t chunkZ)const;
		void UnloadChunk(const Ref<Connection> &connection, int32_t chunkX, int32_t chunkZ);

		static int32_t BlockToChunk(const double blockCoord) {
			return static_cast<int32_t>(std::floor(blockCoord)) >> 4;
		}

		Ref<ChunkGenerator> m_Generator;
		int m_ViewDistance;

		std::mutex m_Mutex;
		std::unordered_map<Connection*, PlayerChunkState> m_PlayerStates;
	};

}
