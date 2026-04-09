#include "axpch.h"
#include "Axiom/Environment/Level/Physics/WorldTicker.h"

#include "Axiom/Core/Log.h"

#include <chrono>

namespace Axiom {

	void WorldTicker::Start() {
		m_Running = true;
		m_Thread = std::thread(&WorldTicker::TickLoop, this);
		AX_CORE_INFO("World physics started");
	}

	void WorldTicker::Stop() {
		m_Running = false;
		if (m_Thread.joinable()) {
			m_Thread.join();
		}
	}

	Result<void> WorldTicker::SetBlock(const int32_t worldX, const int32_t worldY, const int32_t worldZ,
		const int32_t blockState) {

		{
			std::lock_guard<std::mutex> lock(m_BlockMutex);

			// Log when overwriting a fluid with a non-fluid
			auto iterator = m_BlockOverrides.find({worldX, worldY, worldZ});
			if (iterator != m_BlockOverrides.end()) {
				int32_t oldState = iterator->second;
				if (FluidState::IsFluid(oldState) && !FluidState::IsFluid(blockState)) {
					AX_CORE_WARN("Overwriting fluid {} at ({},{},{}) with {}",
						oldState, worldX, worldY, worldZ, blockState);
				}
			}

			m_BlockOverrides[{worldX, worldY, worldZ}] = blockState;
		}

		BroadcastBlockChange(worldX, worldY, worldZ, blockState);
		m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);

		// Mark chunk as dirty for save-on-unload
		if (m_BlockDirtyCallback) {
			m_BlockDirtyCallback(worldX >> 4, worldZ >> 4);
		}

		return {};
	}

	int32_t WorldTicker::GetBlock(const int32_t worldX, const int32_t worldY, const int32_t worldZ) const {
		// Check overrides first (player-placed/broken blocks)
		{
			std::lock_guard<std::mutex> lock(m_BlockMutex);
			const auto iterator = m_BlockOverrides.find({worldX, worldY, worldZ});
			if (iterator != m_BlockOverrides.end()) {
				return iterator->second;
			}
		}

		// Fall back to generated terrain
		if (m_TerrainLookup) {
			return m_TerrainLookup(worldX, worldY, worldZ);
		}

		return BlockState::Air;
	}

	void WorldTicker::ScanChunkForPhysics(const int32_t chunkX, const int32_t chunkZ) {
		if (!m_TerrainLookup) return;

		const int baseX = chunkX * 16;
		const int baseZ = chunkZ * 16;

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = baseX + localX;
				const int worldZ = baseZ + localZ;

				for (int worldY = 0; worldY <= 200; worldY++) {
					const int32_t block = m_TerrainLookup(worldX, worldY, worldZ);

					// Gravity blocks over air
					if (GravityBlocks::IsGravityBlock(block)) {
						const int32_t below = m_TerrainLookup(worldX, worldY - 1, worldZ);
						if (below == BlockState::Air || FluidState::IsFluid(below)) {
							m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);
						}
					}

					// Fluid sources that need to flow: air below or
					// air to the side at the SAME level (not just air above,
					// which is normal for ocean surfaces)
					if (FluidState::IsSource(block)) {
						// Don't auto-flow ocean/lake water at or below sea level.
						// Generated water at sea level is stable — only trigger
						// flow for water above sea level (placed by players or
						// terrain features like elevated pools).
						constexpr int SeaLevel = 63;
						if (worldY <= SeaLevel) continue;

						const int32_t below = m_TerrainLookup(worldX, worldY - 1, worldZ);

						// Water over air = needs to fall
						if (below == BlockState::Air) {
							m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);
							continue;
						}

						// Horizontal flow only if neighbor has a drop
						auto shouldFlow = [&](int neighborX, int neighborZ) -> bool {
							const int32_t neighbor = m_TerrainLookup(neighborX, worldY, neighborZ);
							if (neighbor != BlockState::Air) return false;
							const int32_t neighborBelow = m_TerrainLookup(neighborX, worldY - 1, neighborZ);
							return neighborBelow == BlockState::Air;
						};

						if (shouldFlow(worldX + 1, worldZ) ||
							shouldFlow(worldX - 1, worldZ) ||
							shouldFlow(worldX, worldZ + 1) ||
							shouldFlow(worldX, worldZ - 1)) {
							m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);
						}
					}
				}
			}
		}
	}

	void WorldTicker::TickLoop() {
		using Clock = std::chrono::steady_clock;
		auto nextTick = Clock::now();
		constexpr auto tickDuration = std::chrono::milliseconds(50);

		while (m_Running) {
			nextTick += tickDuration;

			m_BlockPhysics.Tick(
				[this](const int32_t blockX, const int32_t blockY, const int32_t blockZ) -> int32_t {
					return GetBlock(blockX, blockY, blockZ);
				},
				[this](const int32_t blockX, const int32_t blockY, const int32_t blockZ, const int32_t state) {
					std::lock_guard<std::mutex> lock(m_BlockMutex);
					m_BlockOverrides[{blockX, blockY, blockZ}] = state;
				},
				[this](const int32_t blockX, const int32_t blockY, const int32_t blockZ, const int32_t state) {
					BroadcastBlockChange(blockX, blockY, blockZ, state);
				}
			);

			const auto now = Clock::now();
			if (nextTick > now) {
				std::this_thread::sleep_until(nextTick);
			}
		}
	}

	void WorldTicker::BroadcastBlockChange(const int32_t worldX, const int32_t worldY, const int32_t worldZ,
		const int32_t blockState) {

		NetworkBuffer payload;
		payload.WriteBlockPosition(worldX, worldY, worldZ);
		payload.WriteVarInt(blockState);

		const auto players = m_PlayerManager.AllPlayers();
		for (const auto& player : players) {
			const auto connection = player->GetConnection();
			if (!connection || !connection->IsConnected()) continue;
			if (connection->State() != ConnectionState::Play) continue;
			connection->SendRawPacket(Clientbound::Play::BlockUpdate, payload);
		}
	}

}
