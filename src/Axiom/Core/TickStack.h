#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Core/Timestep.h"

#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

namespace Axiom {

/**
 * Manages a stack of Tickable objects with async task support.
 *
 * Runs tickables at 20 TPS (50ms per tick) synchronously, but provides
 * Async() and PostToMainThread() for parallel work.
 *
 * Usage:
 *   TickStack stack;
 *   stack.PushTick(mySystem);
 *   stack.RunSyncLoop([]() { return shouldContinue; });
 */
class TickStack {
public:
	TickStack();
	~TickStack();

	TickStack(const TickStack&) = delete;
	TickStack& operator=(const TickStack&) = delete;

	/**
	 * Add a Tickable to the stack.
	 * Thread-safe.
	 */
	void PushTick(Tickable* tickable);

	/**
	 * Remove a Tickable from the stack.
	 * Thread-safe.
	 */
	void PopTick(Tickable* tickable);

	/**
	 * Execute a function asynchronously on the thread pool.
	 * Thread-safe. Can be called from any thread.
	 *
	 * Example:
	 *   Async([this]() {
	 *       auto result = HeavyComputation();
	 *       PostToMainThread([this, result]() {
	 *           ApplyResult(result);
	 *       });
	 *   });
	 */
	template<typename Func>
	auto Async(Func&& func) -> std::future<decltype(func())> {
		using ReturnType = decltype(func());
		auto promise = std::make_shared<std::promise<ReturnType>>();
		auto future = promise->get_future();

		{
			std::lock_guard<std::mutex> lock(m_TaskMutex);
			m_AsyncTasks.emplace([promise, func = std::forward<Func>(func)]() mutable {
				try {
					if constexpr (std::is_void_v<ReturnType>) {
						func();
						promise->set_value();
					} else {
						promise->set_value(func());
					}
				} catch (...) {
					promise->set_exception(std::current_exception());
				}
			});
		}
		m_TaskCV.notify_one();
		return future;
	}

	/**
	 * Post a task to be executed on the main thread during the next tick.
	 * Thread-safe. Can be called from any thread.
	 */
	void PostToMainThread(std::function<void()> task);

	/**
	 * Run the synchronous tick loop.
	 *
	 * @param shouldContinue Function returning true while loop should continue
	 * @param targetTPS Target ticks per second (default: 20 TPS = 50ms)
	 */
	void RunSyncLoop(std::function<bool()> shouldContinue, float targetTPS = 20.0f);

	/**
	 * Process a single tick manually.
	 * Call this from your own loop if you don't use RunSyncLoop().
	 */
	void Tick(Timestep timestep);

	/**
	 * Get the current tick number.
	 */
	uint64_t GetCurrentTick() const { return m_CurrentTick.load(); }

	/**
	 * Get the actual measured TPS.
	 */
	float GetActualTPS() const { return m_ActualTPS.load(); }

	/**
	 * Check if the tick stack is running.
	 */
	bool IsRunning() const { return m_Running.load(); }

	/**
	 * Stop the tick loop gracefully.
	 */
	void Stop() { m_Running = false; }

	// Iterators for range-based for loops
	std::vector<Tickable*>::iterator begin() { return m_Tickables.begin(); }
	std::vector<Tickable*>::iterator end() { return m_Tickables.end(); }
	std::vector<Tickable*>::reverse_iterator rbegin() { return m_Tickables.rbegin(); }
	std::vector<Tickable*>::reverse_iterator rend() { return m_Tickables.rend(); }
	std::vector<Tickable*>::const_iterator begin() const { return m_Tickables.begin(); }
	std::vector<Tickable*>::const_iterator end()	const { return m_Tickables.end(); }
	std::vector<Tickable*>::const_reverse_iterator rbegin() const { return m_Tickables.rbegin(); }
	std::vector<Tickable*>::const_reverse_iterator rend() const { return m_Tickables.rend(); }

private:
	void ProcessMainThreadTasks();
	void ExecuteAsyncTasks();
	void SortTickablesByPhase();
	void WorkerThreadLoop();

private:
	std::vector<Tickable*> m_Tickables;
	std::mutex m_TickablesMutex;
	bool m_TickablesDirty = false;

	// Async task queue
	std::queue<std::function<void()>> m_AsyncTasks;
	std::mutex m_TaskMutex;
	std::condition_variable m_TaskCV;
	std::vector<std::thread> m_WorkerThreads;
	std::atomic<bool> m_StopWorkers{false};

	// Main thread task queue (results from async)
	std::queue<std::function<void()>> m_MainThreadTasks;
	std::mutex m_MainThreadMutex;

	// Timing
	std::atomic<uint64_t> m_CurrentTick{0};
	std::atomic<float> m_ActualTPS{20.0f};
	std::atomic<bool> m_Running{false};
	float m_TargetDeltaTime = 0.05f; // 50ms for 20 TPS

	// TPS calculation
	std::chrono::steady_clock::time_point m_LastTPSCalcTime;
	uint64_t m_TickCountSinceLastCalc = 0;
};

} // namespace Axiom
