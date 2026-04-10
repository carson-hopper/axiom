#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"

#include <atomic>
#include <cstdint>
#include <thread>

namespace Axiom {

	enum class WeatherType {
		Clear,
		Rain,
		Thunder
	};

	/**
	 * Manages world time and weather, broadcasting updates to players.
	 * Runs on its own thread at 20 ticks per second.
	 */
	class LevelTime {
	public:
		static constexpr int64_t TicksPerDay = 24000;
		static constexpr int64_t Sunrise = 0;
		static constexpr int64_t Noon = 6000;
		static constexpr int64_t Sunset = 12000;
		static constexpr int64_t Midnight = 18000;

		explicit LevelTime(class NetworkServer& server)
			: m_Server(server) {}

		~LevelTime() { Stop(); }

		LevelTime(const LevelTime&) = delete;
		LevelTime& operator=(const LevelTime&) = delete;

		void Start();
		void Stop();

		// ----- Time -----------------------------------------------------

		int64_t WorldAge() const { return m_WorldAge; }
		int64_t TimeOfDay() const { return m_TimeOfDay; }

		void SetTimeOfDay(int64_t time);
		void SetTimeFrozen(bool frozen) { m_TimeFrozen = frozen; }
		bool IsTimeFrozen() const { return m_TimeFrozen; }

		// ----- Weather --------------------------------------------------

		WeatherType Weather() const { return m_Weather; }
		void SetWeather(WeatherType weather);

	private:
		void TickLoop();
		void BroadcastTime();
		void BroadcastWeatherChange(WeatherType newWeather);

		NetworkServer& m_Server;

		std::atomic<int64_t> m_WorldAge = 0;
		std::atomic<int64_t> m_TimeOfDay = 6000; // Start at noon
		std::atomic<bool> m_TimeFrozen = false;
		std::atomic<WeatherType> m_Weather = WeatherType::Clear;

		std::thread m_Thread;
		std::atomic<bool> m_Running = false;
		int m_TickCounter = 0;
	};

}
