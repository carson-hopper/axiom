#pragma once

#include <chrono>
#include <compare>
#include <cstdint>

namespace Axiom {

	/**
	 * Wall-clock point in time, stored as
	 * unix seconds since the epoch. Use
	 * Time::Now() to get the current instant.
	 *
	 * Fluent AddX methods shift an instance
	 * forward (or backward with a negative
	 * argument) and return a new Time:
	 *
	 *   auto expiry = Time::Now().AddDays(7);
	 *   if (expiry.HasPassed()) { ... }
	 */
	class Time {
	public:
		constexpr Time() = default;
		explicit constexpr Time(const int64_t unixSeconds)
			: m_Seconds(unixSeconds) {}

		/**
		 * Current wall-clock time. Safe to
		 * persist to disk; survives restarts.
		 */
		static Time Now() {
			const auto now = std::chrono::system_clock::now();
			return Time(std::chrono::duration_cast<std::chrono::seconds>(
				now.time_since_epoch()).count());
		}

		/**
		 * Raw unix seconds since the epoch.
		 */
		constexpr int64_t UnixSeconds() const { return m_Seconds; }

		/**
		 * True when this time is the default
		 * zero sentinel (never-set / never-expire).
		 */
		constexpr bool IsZero() const { return m_Seconds == 0; }

		/**
		 * True when this time has already
		 * elapsed relative to Now().
		 */
		bool HasPassed() const {
			return Now().m_Seconds >= m_Seconds;
		}

		constexpr bool operator==(const Time&) const = default;
		constexpr auto operator<=>(const Time&) const = default;

		/** Difference between two instants, in seconds. */
		constexpr int64_t operator-(const Time& other) const {
			return m_Seconds - other.m_Seconds;
		}

		// ----- Fluent duration shifts ------------------------------

		/** Return a copy shifted by N seconds. */
		constexpr Time AddSeconds(const int64_t count = 1) const {
			return Time(m_Seconds + count);
		}

		/** Return a copy shifted by N minutes. */
		constexpr Time AddMinutes(const int64_t count = 1) const {
			return Time(m_Seconds + count * 60);
		}

		/** Return a copy shifted by N hours. */
		constexpr Time AddHours(const int64_t count = 1) const {
			return Time(m_Seconds + count * 60 * 60);
		}

		/** Return a copy shifted by N days. */
		constexpr Time AddDays(const int64_t count = 1) const {
			return Time(m_Seconds + count * 24 * 60 * 60);
		}

		// ----- Frame timing (process-relative) ---------------------

		/**
		 * Seconds since the first call. For
		 * delta-time / frame timing only; do
		 * not persist — use Now() for that.
		 */
		static float GetTime() {
			static const auto start = std::chrono::high_resolution_clock::now();
			const auto now = std::chrono::high_resolution_clock::now();
			return static_cast<float>(
				std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count())
				* 0.001f * 0.001f * 0.001f;
		}

	private:
		int64_t m_Seconds = 0;
	};

}
