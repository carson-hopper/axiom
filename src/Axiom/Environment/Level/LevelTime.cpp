#include "axpch.h"
#include "Axiom/Environment/Level/LevelTime.h"

#include "Axiom/Core/Log.h"

#include <chrono>

namespace Axiom {

	void LevelTime::Start() {
		m_Running = true;
		m_Thread = std::thread(&LevelTime::TickLoop, this);
		AX_CORE_INFO("World time started (time of day: {})", m_TimeOfDay.load());
	}

	void LevelTime::Stop() {
		m_Running = false;
		if (m_Thread.joinable()) {
			m_Thread.join();
		}
	}

	void LevelTime::SetWeather(WeatherType weather) {
		WeatherType previous = m_Weather.exchange(weather);
		if (previous != weather) {
			BroadcastWeatherChange(weather);
		}
	}

	void LevelTime::TickLoop() {
		using Clock = std::chrono::steady_clock;
		auto nextTick = Clock::now();
		constexpr auto tickDuration = std::chrono::milliseconds(50); // 20 TPS

		while (m_Running) {
			nextTick += tickDuration;

			m_WorldAge++;

			if (!m_TimeFrozen) {
				m_TimeOfDay = (m_TimeOfDay + 1) % TicksPerDay;
			}

			m_TickCounter++;

			// Broadcast time every 20 ticks (1 second)
			if (m_TickCounter >= 20) {
				m_TickCounter = 0;
				BroadcastTime();
			}

			auto now = Clock::now();
			if (nextTick > now) {
				std::this_thread::sleep_until(nextTick);
			}
		}
	}

	void LevelTime::BroadcastTime() {
		auto players = m_PlayerManager.AllPlayers();

		for (const auto& player : players) {
			auto connection = player->GetConnection();
			if (!connection || !connection->IsConnected()) continue;
			if (connection->State() != ConnectionState::Play) continue;

			NetworkBuffer payload;
			payload.WriteLong(m_WorldAge);  // Game time

			// Clock updates map: 1 entry (overworld clock)
			payload.WriteVarInt(1);

			// WorldClock registry ID for overworld: index 0 (holderRegistry uses raw ID)
			payload.WriteVarInt(0);

			// ClockNetworkState: totalTicks (VarLong), partialTick (Float), rate (Float)
			payload.WriteVarLong(m_TimeOfDay);
			payload.WriteFloat(0.0f);  // partialTick
			payload.WriteFloat(m_TimeFrozen ? 0.0f : 1.0f);  // rate

			connection->SendRawPacket(Clientbound::Play::SetTime, payload);
		}
	}

	void LevelTime::BroadcastWeatherChange(WeatherType newWeather) {
		auto players = m_PlayerManager.AllPlayers();

		for (const auto& player : players) {
			auto connection = player->GetConnection();
			if (!connection || !connection->IsConnected()) continue;
			if (connection->State() != ConnectionState::Play) continue;

			// Game event 1 = Begin raining, 2 = End raining
			// Game event 7 = Rain level, 8 = Thunder level
			switch (newWeather) {
				case WeatherType::Clear: {
					// End rain
					NetworkBuffer endRain;
					endRain.WriteByte(2); // End raining
					endRain.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, endRain);

					// Set rain level to 0
					NetworkBuffer rainLevel;
					rainLevel.WriteByte(7);
					rainLevel.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, rainLevel);

					// Set thunder level to 0
					NetworkBuffer thunderLevel;
					thunderLevel.WriteByte(8);
					thunderLevel.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, thunderLevel);
					break;
				}
				case WeatherType::Rain: {
					// Begin rain
					NetworkBuffer beginRain;
					beginRain.WriteByte(1);
					beginRain.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, beginRain);

					NetworkBuffer rainLevel;
					rainLevel.WriteByte(7);
					rainLevel.WriteFloat(1.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, rainLevel);

					NetworkBuffer thunderLevel;
					thunderLevel.WriteByte(8);
					thunderLevel.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, thunderLevel);
					break;
				}
				case WeatherType::Thunder: {
					NetworkBuffer beginRain;
					beginRain.WriteByte(1);
					beginRain.WriteFloat(0.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, beginRain);

					NetworkBuffer rainLevel;
					rainLevel.WriteByte(7);
					rainLevel.WriteFloat(1.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, rainLevel);

					NetworkBuffer thunderLevel;
					thunderLevel.WriteByte(8);
					thunderLevel.WriteFloat(1.0f);
					connection->SendRawPacket(Clientbound::Play::GameEvent, thunderLevel);
					break;
				}
			}
		}
	}

}
