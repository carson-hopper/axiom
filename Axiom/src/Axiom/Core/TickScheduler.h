#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Timestep.h"
#include "Axiom/Core/TaskQueue.h"
#include "Axiom/Core/TickRegistry.h"
#include "Axiom/Core/AsyncExecutor.h"

#include <atomic>
#include <chrono>
#include <functional>

namespace Axiom {

/**
 * Main tick loop with two-phase model:
 * synchronous tickables followed by
 * async dispatch, wait, and
 * merge.
 */
class TickScheduler {
public:
	TickScheduler(TickRegistry& registry, TaskQueue& taskQueue, AsyncExecutor& executor);

	/**
	 * Run the synchronous tick loop
	 * with sleep-based timing.
	 */
	void RunSyncLoop(const std::function<bool()>& shouldContinue, float targetTPS = 20.0f);

	/**
	 * Execute a single tick with
	 * the full two-phase
	 * pipeline.
	 */
	void Tick(Timestep timestep);

	/**
	 * Signal the loop
	 * to stop.
	 */
	void Stop() { m_Running = false; }

	/**
	 * Current monotonic
	 * tick number.
	 */
	uint64_t CurrentTick() const { return m_CurrentTick.load(); }

	/**
	 * Measured ticks per
	 * second.
	 */
	float ActualTPS() const { return m_ActualTPS.load(); }

	/**
	 * Whether the loop is
	 * currently running.
	 */
	bool IsRunning() const { return m_Running.load(); }

private:
	TickRegistry& m_Registry;
	TaskQueue& m_TaskQueue;
	AsyncExecutor& m_Executor;

	std::atomic<uint64_t> m_CurrentTick{0};
	std::atomic<float> m_ActualTPS{20.0f};
	std::atomic<bool> m_Running{false};

	std::chrono::steady_clock::time_point m_LastTPSCalcTime;
	uint64_t m_TickCountSinceLastCalc = 0;
};

}
