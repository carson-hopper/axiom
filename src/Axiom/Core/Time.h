#pragma once

#include <chrono>

namespace Axiom {

	/**
	 * Static time utility for getting elapsed time since start.
	 */
	class Time {
	public:
		/**
		 * Get time in seconds since the first call to GetTime.
		 */
		static float GetTime() {
			static const auto start = std::chrono::high_resolution_clock::now();
			const auto now = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count()
				   * 0.001f * 0.001f * 0.001f;
		}
	};

}
