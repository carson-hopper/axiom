#include "WorldTicker.h"

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

	void WorldTicker::OnBlockChanged(const int32_t worldX, const int32_t worldY, const int32_t worldZ,
		const int32_t newBlockState) {

		{
			std::lock_guard<std::mutex> lock(m_BlockMutex);
			m_BlockOverrides[{worldX, worldY, worldZ}] = newBlockState;
		}

		m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);
	}

	void WorldTicker::SetBlock(const int32_t worldX, const int32_t worldY, const int32_t worldZ,
		const int32_t blockState) {

		{
			std::lock_guard<std::mutex> lock(m_BlockMutex);
			m_BlockOverrides[{worldX, worldY, worldZ}] = blockState;
		}

		BroadcastBlockChange(worldX, worldY, worldZ, blockState);
		m_BlockPhysics.ScheduleUpdate(worldX, worldY, worldZ);
	}

	int32_t WorldTicker::GetBlock(const int32_t worldX, const int32_t worldY, const int32_t worldZ) const {
		std::lock_guard<std::mutex> lock(m_BlockMutex);
		const auto iterator = m_BlockOverrides.find({worldX, worldY, worldZ});
		if (iterator != m_BlockOverrides.end()) {
			return iterator->second;
		}
		return BlockState::Air;
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

		// Encode position as packed Long (same as Position type in MC protocol)
		const int64_t encodedPosition = (static_cast<int64_t>(worldX & 0x3FFFFFF) << 38)
			| (static_cast<int64_t>(worldZ & 0x3FFFFFF) << 12)
			| static_cast<int64_t>(worldY & 0xFFF);

		NetworkBuffer payload;
		payload.WriteLong(encodedPosition);
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
