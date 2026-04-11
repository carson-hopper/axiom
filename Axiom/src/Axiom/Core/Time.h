#pragma once

#include <chrono>
#include <compare>
#include <cstdint>

namespace Axiom {

	class Time {
	public:
		constexpr Time() = default;
		explicit constexpr Time(const int64_t unixSeconds)
			: m_Seconds(unixSeconds) {}

		static Time Now() {
			const auto now = std::chrono::system_clock::now();
			return Time(std::chrono::duration_cast<std::chrono::seconds>(
				now.time_since_epoch()).count());
		}

		constexpr int64_t UnixSeconds() const { return m_Seconds; }
		constexpr bool IsZero() const { return m_Seconds == 0; }

		bool HasPassed() const {
			return Now().m_Seconds > m_Seconds;
		}

		constexpr bool operator==(const Time&) const = default;
		constexpr auto operator<=>(const Time&) const = default;

		constexpr int64_t operator-(const Time& other) const {
			return m_Seconds - other.m_Seconds;
		}

		constexpr Time Copy() const { return *this; }

		constexpr Time& AddSeconds(const int64_t count = 1) {
			m_Seconds = SaturatingAdd(m_Seconds, count);
			return *this;
		}

		constexpr Time& AddMinutes(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 60));
		}

		constexpr Time& AddHours(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 3600));
		}

		constexpr Time& AddDays(const int64_t count = 1) {
			return AddSeconds(SaturatingMul(count, 86400));
		}

		// Seconds since first call. Delta-time / frame timing only;
		// never persist — use Now() for wall-clock instants.
		static float GetTime() {
			static const auto start = std::chrono::high_resolution_clock::now();
			const auto now = std::chrono::high_resolution_clock::now();
			return static_cast<float>(
				std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count())
				* 0.001f * 0.001f * 0.001f;
		}

	private:
		static constexpr int64_t SaturatingAdd(const int64_t a, const int64_t b) {
			if (b > 0 && a > INT64_MAX - b) {
				return INT64_MAX;
			}
			if (b < 0 && a < INT64_MIN - b) {
				return INT64_MIN;
			}
			return a + b;
		}

		static constexpr int64_t SaturatingMul(const int64_t a, const int64_t b) {
			if (a == 0 || b == 0) {
				return 0;
			}
			// INT64_MIN has no positive counterpart, so -INT64_MIN is UB.
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
