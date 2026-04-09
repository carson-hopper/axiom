#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Core/AsyncTickable.h"

#include <mutex>
#include <vector>

namespace Axiom {

/**
 * Manages the collection of Tickable and
 * AsyncTickable objects, providing
 * thread-safe registration and
 * sorted snapshots.
 */
class TickRegistry {
public:
	/**
	 * Register a synchronous tickable.
	 * Thread-safe.
	 */
	void RegisterTickable(Ref<Tickable> tickable);

	/**
	 * Unregister a synchronous tickable
	 * by raw pointer. Thread-safe.
	 */
	void UnregisterTickable(Tickable* tickable);

	/**
	 * Register an async tickable.
	 * Thread-safe.
	 */
	void RegisterAsyncTickable(Ref<AsyncTickable> asyncTickable);

	/**
	 * Unregister an async tickable
	 * by raw pointer.
	 * Thread-safe.
	 */
	void UnregisterAsyncTickable(AsyncTickable* asyncTickable);

	/**
	 * Get sorted snapshot of enabled
	 * tickables (by tick phase).
	 */
	std::vector<Ref<Tickable>> GetTickables();

	/**
	 * Get snapshot of enabled
	 * async tickables.
	 */
	std::vector<Ref<AsyncTickable>> GetAsyncTickables();

private:
	std::vector<Ref<Tickable>> m_Tickables;
	std::vector<Ref<AsyncTickable>> m_AsyncTickables;
	std::mutex m_Mutex;
	bool m_TickablesDirty = false;
};

}
