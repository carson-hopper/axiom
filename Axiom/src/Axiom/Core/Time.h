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
	 * Fluent AddX methods shift this instance
	 * IN PLACE (forward, or backward with a
	 * negative argument) and return `*this`:
	 *
	 *   auto expiry = Time::Now().AddDays(7);
	 *   if (expiry.HasPassed()) { ... }
	 *
	 * When you want to branch off without
	 * mutating the original, use `Copy()`:
	 *
	 *   const auto soft = ban.Copy().AddDays(1);
	 *   const auto hard = ban.Copy().AddDays(7);
	 *
	 * All shifts saturate at INT64_MAX /
	 * INT64_MIN instead of wrapping, so
	 * far-future / far-past computations
	 * never silently overflow.
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
			return Now().m_Seconds > m_Seconds;
		}

		constexpr bool operator==(const Time&) const = default;
		constexpr auto operator<=>(const Time&) const = default;

		/** Difference between two instants, in seconds. */
		constexpr int64_t operator-(const Time& other) const {
			return m_Seconds - other.m_Seconds;
		}

		/**
		 * Return a detached copy of this
		 * instant. Use when you want to
		 * branch off a fluent AddX chain
		 * without mutating the original.
		 */
		constexpr Time Copy() const { return *this; }

		// ----- Fluent duration shifts ------------------------------

		/**
		 * Shift this instant forward by N
		 * seconds in place (or backward
		 * when N is negative). Returns
		 * `*this` so calls can be chained.
		 * Saturates cleanly at INT64_MAX /
		 * INT64_MIN instead of wrapping.
		 */
		constexpr Time& AddSeconds(const int64_t count = 1) {
			m_Seconds = SaturatingAdd(m_Seconds, count);
			return *this;
		}

		/** Shift forward by N minutes (backward if negative). */
		constexpr Time& AddMinutes(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 60));
		}

		/** Shift forward by N hours (backward if negative). */
		constexpr Time& AddHours(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 3600));
		}

		/** Shift forward by N days (backward if negative). */
		constexpr Time& AddDays(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 86400));
		}

		// ----- Frame timing (process-relative) ---------------------

		/**
		 * Seconds since first call to GetTime.
		 * For delta-time / frame timing only.
		 * Never persist — use Now() for
		 * wall-clock instants.
		 */
		static float GetTime() {
			static const auto start = std::chrono::high_resolution_clock::now();
			const auto now = std::chrono::high_resolution_clock::now();
			return static_cast<float>(
				std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count())
				* 0.001f * 0.001f * 0.001f;
		}

	private:
		/**
		 * Saturating 64-bit add. Overflow
		 * clamps to INT64_MAX / INT64_MIN
		 * instead of wrapping (which would
		 * be undefined behaviour for signed
		 * integers in C++).
		 */
		static constexpr int64_t SaturatingAdd(const int64_t a, const int64_t b) {
			if (b > 0 && a > INT64_MAX - b) {
				return INT64_MAX;
			}
			if (b < 0 && a < INT64_MIN - b) {
				return INT64_MIN;
			}
			return a + b;
		}

		/**
		 * Saturating 64-bit multiply.
		 * Overflow clamps to INT64_MAX /
		 * INT64_MIN instead of wrapping.
		 * Also guards against the
		 * `INT64_MIN * -1` UB corner.
		 */
		static constexpr int64_t SaturatingMul(const int64_t a, const int64_t b) {
			if (a == 0 || b == 0) {
				return 0;
			}
			// INT64_MIN has no positive counterpart, so
			// -INT64_MIN is UB. Catch both argument slots
			// before we try to take absolute values.
			if (a == INT64_MIN) {
				return b > 0 ? INT64_MIN : INT64_MAX;
			}
			if (b == INT64_MIN) {
				return a > 0 ? INT64_MIN : INT64_MAX;
			}
			const bool negative = (a < 0) != (b < 0);
			const int64_t absA = a < 0 ? -a : a;
			const int64_t absB = b < 0 ? -b : b;
			if (absA > INT64_MAX / absB) {
				return negative ? INT64_MIN : INT64_MAX;
			}
			return a * b;
		}

		int64_t m_Seconds = 0;
	};

}
