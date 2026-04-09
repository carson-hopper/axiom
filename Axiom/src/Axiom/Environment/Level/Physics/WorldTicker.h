#pragma once

#include "Axiom/Core/Error.h"
#include "Axiom/Environment/Level/Physics/BlockPhysics.h"
#include "Axiom/Network/Connection.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace Axiom {

	/**
	 * Drives block physics at 20 TPS and broadcasts block changes
	 * to all nearby players.
	 *
	 * Maintains a sparse override map of block changes on top of the
	 * generated terrain. For any position without an override, queries
	 * the terrain lookup function to determine the generated block.
	 */
	class WorldTicker {
	public:
		using TerrainLookup = std::function<int32_t(int32_t, int32_t, int32_t)>;

		WorldTicker(class NetworkServer& server)
			: m_Server(server) {}

		~WorldTicker() { Stop(); }

		WorldTicker(const WorldTicker&) = delete;
		WorldTicker& operator=(const WorldTicker&) = delete;

		/**
		 * Set the terrain lookup function. Called to get the generated block
		 * state at any position. Must be thread-safe.
		 */
		void SetTerrainLookup(TerrainLookup lookup) {
			m_TerrainLookup = std::move(lookup);
		}

		void Start();
		void Stop();

		/**
		 * Set a block in the world.
		 *
		 * @param worldX World X coordinate
		 * @param worldY World Y coordinate
		 * @param worldZ World Z coordinate
		 * @param blockState Block state ID to set
		 * @return Result<void> Success or error code
		 */
		Result<void> SetBlock(int32_t worldX, int32_t worldY, int32_t worldZ, int32_t blockState);
		int32_t GetBlock(int32_t worldX, int32_t worldY, int32_t worldZ) const;

		/**
		 * Scan a generated chunk for unstable blocks (gravity blocks over air,
		 * exposed fluids) and schedule them for physics. Call after sending
		 * a chunk to ensure natural terrain physics runs.
		 */
		void ScanChunkForPhysics(int32_t chunkX, int32_t chunkZ);

		using BlockDirtyCallback = std::function<void(int32_t chunkX, int32_t chunkZ)>;
		void SetBlockDirtyCallback(BlockDirtyCallback callback) { m_BlockDirtyCallback = std::move(callback); }

		BlockPhysics& Physics() { return m_BlockPhysics; }

	private:
		void TickLoop();
		void BroadcastBlockChange(int32_t worldX, int32_t worldY, int32_t worldZ, int32_t blockState);

		NetworkServer& m_Server;
		BlockPhysics m_BlockPhysics;
		TerrainLookup m_TerrainLookup;
		BlockDirtyCallback m_BlockDirtyCallback;

		mutable std::mutex m_BlockMutex;
		std::unordered_map<BlockPosition, int32_t, BlockPosition::Hash> m_BlockOverrides;

		std::thread m_Thread;
		std::atomic<bool> m_Running = false;
	};

}
