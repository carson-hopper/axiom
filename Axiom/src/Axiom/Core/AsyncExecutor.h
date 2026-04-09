#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/AsyncTickable.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace Axiom {

/**
 * Thread pool that dispatches AsyncTickable
 * work across background threads.
 */
class AsyncExecutor {
public:
	/**
	 * Create the executor with the given
	 * thread count. Zero means auto
	 * (hardware_concurrency - 2,
	 * minimum 1).
	 */
	explicit AsyncExecutor(int threadCount = 0);
	~AsyncExecutor();

	AsyncExecutor(const AsyncExecutor&) = delete;
	AsyncExecutor& operator=(const AsyncExecutor&) = delete;

	/**
	 * Submit a generic async task.
	 * Returns a future for the
	 * result.
	 */
	template<typename Func>
	auto Submit(Func&& func) -> std::future<decltype(func())> {
		using ReturnType = decltype(func());
		auto promise = std::make_shared<std::promise<ReturnType>>();
		auto future = promise->get_future();
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Tasks.emplace([promise, func = std::forward<Func>(func)]() mutable {
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
		m_ConditionVariable.notify_one();
		return future;
	}

	/**
	 * Dispatch a batch of AsyncTickable
	 * TickAsync() calls and block
	 * until all complete.
	 */
	void DispatchAndWait(std::vector<Ref<AsyncTickable>>& asyncTickables);

	/**
	 * Get the number of worker
	 * threads.
	 */
	int ThreadCount() const { return static_cast<int>(m_Workers.size()); }

	/**
	 * Shutdown the executor
	 * and join all threads.
	 */
	void Shutdown();

private:
	void WorkerLoop();

	std::vector<std::thread> m_Workers;
	std::queue<std::function<void()>> m_Tasks;
	std::mutex m_Mutex;
	std::condition_variable m_ConditionVariable;
	std::atomic<bool> m_Stopping{false};
};

}
