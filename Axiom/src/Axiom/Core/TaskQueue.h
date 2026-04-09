#pragma once

#include "Axiom/Core/Base.h"

#include <functional>
#include <mutex>
#include <queue>

namespace Axiom {

/**
 * Thread-safe task queue for posting
 * work to the main thread.
 */
class TaskQueue {
public:
	/**
	 * Enqueue a task for later
	 * execution on the
	 * main thread.
	 */
	void Post(std::function<void()> task);

	/**
	 * Execute and drain all
	 * queued tasks. Must be
	 * called from main
	 * thread.
	 */
	void ProcessAll();

	/**
	 * Check whether the
	 * queue is empty.
	 */
	bool IsEmpty() const;

	/**
	 * Number of tasks
	 * waiting.
	 */
	size_t Size() const;

private:
	std::queue<std::function<void()>> m_Tasks;
	mutable std::mutex m_Mutex;
};

}
