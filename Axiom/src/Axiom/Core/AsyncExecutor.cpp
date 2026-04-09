#include "axpch.h"
#include "Axiom/Core/AsyncExecutor.h"

#include "Axiom/Core/Log.h"

#include <algorithm>
#include <latch>

namespace Axiom {

	AsyncExecutor::AsyncExecutor(int threadCount) {
		if (threadCount <= 0) {
			int hardwareConcurrency = static_cast<int>(std::thread::hardware_concurrency());
			threadCount = std::max(1, hardwareConcurrency - 2);
		}

		m_Workers.reserve(threadCount);
		for (int index = 0; index < threadCount; index++) {
			m_Workers.emplace_back([this] {
				WorkerLoop();
			});
		}

		AX_CORE_INFO("AsyncExecutor created with {} worker threads", threadCount);
	}

	AsyncExecutor::~AsyncExecutor() {
		Shutdown();
	}

	void AsyncExecutor::DispatchAndWait(std::vector<Ref<AsyncTickable>>& asyncTickables) {
		if (asyncTickables.empty()) {
			return;
		}

		std::latch completionLatch(static_cast<std::ptrdiff_t>(asyncTickables.size()));

		for (auto& asyncTickable : asyncTickables) {
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_Tasks.emplace([&asyncTickable, &completionLatch]() {
					try {
						asyncTickable->TickAsync();
					} catch (const std::exception& exception) {
						AX_CORE_ERROR("AsyncTickable::TickAsync() threw: {}", exception.what());
					} catch (...) {
						AX_CORE_ERROR("AsyncTickable::TickAsync() threw unknown exception");
					}
					completionLatch.count_down();
				});
			}
			m_ConditionVariable.notify_one();
		}

		completionLatch.wait();
	}

	void AsyncExecutor::Shutdown() {
		if (m_Stopping.exchange(true)) {
			return;
		}

		m_ConditionVariable.notify_all();

		for (auto& worker : m_Workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}

		AX_CORE_INFO("AsyncExecutor shut down");
	}

	void AsyncExecutor::WorkerLoop() {
		while (true) {
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_ConditionVariable.wait(lock, [this] {
					return !m_Tasks.empty() || m_Stopping;
				});

				if (m_Stopping && m_Tasks.empty()) {
					return;
				}

				task = std::move(m_Tasks.front());
				m_Tasks.pop();
			}

			if (task) {
				task();
			}
		}
	}

}
