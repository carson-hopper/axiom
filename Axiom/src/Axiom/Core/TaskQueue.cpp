#include "axpch.h"
#include "Axiom/Core/TaskQueue.h"

namespace Axiom {

	void TaskQueue::Post(std::function<void()> task) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Tasks.push(std::move(task));
	}

	void TaskQueue::ProcessAll() {
		std::queue<std::function<void()>> tasks;

		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			tasks.swap(m_Tasks);
		}

		while (!tasks.empty()) {
			tasks.front()();
			tasks.pop();
		}
	}

	bool TaskQueue::IsEmpty() const {
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_Tasks.empty();
	}

	size_t TaskQueue::Size() const {
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_Tasks.size();
	}

}
