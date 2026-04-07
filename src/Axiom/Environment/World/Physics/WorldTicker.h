#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/World/Physics/BlockPhysics.h"
#include "Axiom/Environment/World/World.h"
#include "Axiom/Environment/Entity/PlayerManager.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

namespace Axiom {

	/**
	 * Drives block physics at 20 TPS and broadcasts block changes
	 * to all nearby players.
	 */
	class WorldTicker {
	public:
		WorldTicker(PlayerManager& playerManager)
			: m_PlayerManager(playerManager) {}

		~WorldTicker() { Stop(); }

		WorldTicker(const WorldTicker&) = delete;
		WorldTicker& operator=(const WorldTicker&) = delete;

		void Start();
		void Stop();

		/**
		 * Notify the ticker that a block changed (e.g., player placed/broke).
		 * This schedules physics updates for neighbors.
		 */
		void OnBlockChanged(int32_t worldX, int32_t worldY, int32_t worldZ,
			int32_t newBlockState);

		/**
		 * Set a block in the world and notify physics + clients.
		 */
		void SetBlock(int32_t worldX, int32_t worldY, int32_t worldZ,
			int32_t blockState);

		/**
		 * Get a block from world storage.
		 */
		int32_t GetBlock(int32_t worldX, int32_t worldY, int32_t worldZ) const;

		BlockPhysics& Physics() { return m_BlockPhysics; }

	private:
		void TickLoop();
		void BroadcastBlockChange(int32_t worldX, int32_t worldY, int32_t worldZ,
			int32_t blockState);

		PlayerManager& m_PlayerManager;
		BlockPhysics m_BlockPhysics;

		// Simple block storage for physics (chunk-less, position → state map)
		mutable std::mutex m_BlockMutex;
		std::unordered_map<BlockPosition, int32_t, BlockPosition::Hash> m_BlockOverrides;

		std::thread m_Thread;
		std::atomic<bool> m_Running = false;
	};

}
