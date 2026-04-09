#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

namespace Axiom {

/**
 * Monitors tick duration on a background
 * thread and logs warnings when ticks
 * exceed a configurable time
 * limit.
 */
class Watchdog {
public:
	/**
	 * Create the watchdog with a maximum
	 * allowed tick duration in
	 * milliseconds.
	 */
	explicit Watchdog(int64_t maxTickTimeMillis = 60000);
	~Watchdog();

	Watchdog(const Watchdog&) = delete;
	Watchdog& operator=(const Watchdog&) = delete;

	/**
	 * Record that a new tick
	 * has started.
	 */
	void TickStarted();

	/**
	 * Record that the current
	 * tick has finished.
	 */
	void TickFinished();

	/**
	 * Start the watchdog
	 * monitor thread.
	 */
	void Start();

	/**
	 * Stop the watchdog
	 * monitor thread.
	 */
	void Stop();

private:
	void MonitorLoop();

	std::thread m_Thread;
	std::atomic<bool> m_Running{false};
	std::atomic<std::chrono::steady_clock::time_point> m_LastTickStart;
	std::atomic<bool> m_InTick{false};
	int64_t m_MaxTickTimeMillis;
	std::mutex m_Mutex;
	std::condition_variable m_ConditionVariable;
};

}
