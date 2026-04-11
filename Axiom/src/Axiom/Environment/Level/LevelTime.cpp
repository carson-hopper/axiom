#include "axpch.h"
#include "Axiom/Environment/Level/LevelTime.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Event/EventBus.h"
#include "Axiom/Event/LevelEvents.h"
#include "Axiom/Network/NetworkServer.h"

#include <algorithm>
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
			LevelWeatherChangedEvent event(previous, weather);
			Application::Instance().Events().Publish(event);
		}
	}

	void LevelTime::SetTimeOfDay(int64_t time) {
		// Clamp into the valid [0, TicksPerDay) window before
		// touching the atomic so out-of-range inputs cannot
		// poison the stored value or leak past the event.
		const int64_t clamped = std::clamp<int64_t>(time, 0, TicksPerDay - 1);
		const int64_t previous = m_TimeOfDay.exchange(clamped);
		if (previous != clamped) {
			LevelTimeChangedEvent event(previous, clamped);
			Application::Instance().Events().Publish(event);
		}
	}

	void LevelTime::TickLoop() {
		using Clock = std::chrono::steady_clock;
		auto nextTick = Clock::now();
		constexpr auto tickDuration = std::chrono::milliseconds(50); // 20 TPS

		/**
		 * Hard cap on catch-up ticks after a long stall.
		 * If the loop ever falls this many ticks behind
		 * (~1 second of wall-clock drift), we reset the
		 * schedule to "now + one tick" instead of firing
		 * every missed tick in a tight burst. Without
		 * this, a 5-second pause (GC, debugger break,
		 * kernel scheduler hiccup) would cause 100 ticks
		 * to fire back-to-back and drown every other
		 * consumer of the event bus.
		 */
		constexpr int MaxCatchupTicks = 20;

		while (m_Running) {
			nextTick += tickDuration;

			// Relaxed is fine here: m_WorldAge is a
			// monotonic counter with no happens-before
			// dependencies. Readers either see the new
			// value or the old one, which is all any
			// consumer needs. seq-cst was overkill and
			// added needless cross-core synchronisation
			// on every tick.
			m_WorldAge.fetch_add(1, std::memory_order_relaxed);

			if (!m_TimeFrozen) {
				m_TimeOfDay = (m_TimeOfDay + 1) % TicksPerDay;
			}

			m_TickCounter++;

			// Broadcast time every 20 ticks (1 second)
			if (m_TickCounter >= 20) {
				m_TickCounter = 0;
				BroadcastTime();
			}

			// Drift correction: if nextTick has fallen
			// more than MaxCatchupTicks behind wall-clock,
			// snap forward instead of firing the backlog
			// in a burst. This keeps tick rate steady
			// after a long stall and prevents a "tick
			// storm" from overwhelming the event bus.
			auto now = Clock::now();
			const auto behind = now - nextTick;
			if (behind > MaxCatchupTicks * tickDuration) {
				const auto missed = behind / tickDuration;
				AX_CORE_WARN("LevelTime fell {} ticks behind; resetting schedule",
					static_cast<int64_t>(missed));
				nextTick = now + tickDuration;
				continue;
			}

			if (nextTick > now) {
				std::this_thread::sleep_until(nextTick);
			}
		}
	}

	void LevelTime::BroadcastTime() {
		auto players = m_Server.AllPlayers();

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
		auto players = m_Server.AllPlayers();

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
