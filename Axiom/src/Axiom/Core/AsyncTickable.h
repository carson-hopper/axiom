#pragma once

#include "Axiom/Core/Timestep.h"
#include "Axiom/Utilities/Memory/Ref.h"

namespace Axiom {

/**
 * Interface for tick-aligned background work
 * with a three-phase lifecycle: prepare on
 * main thread, execute async, merge on
 * main thread.
 */
class AsyncTickable : public virtual RefCounted {
public:
	virtual ~AsyncTickable() = default;

	/**
	 * Called on the main thread to snapshot
	 * state before the async phase.
	 */
	virtual void PrepareAsync(Timestep timestep) = 0;

	/**
	 * Runs on background executor. Must
	 * NOT touch game state.
	 */
	virtual void TickAsync() = 0;

	/**
	 * Called on main thread to merge
	 * async results back into
	 * game state.
	 */
	virtual void MergeResults() = 0;

	/**
	 * Whether this async tickable
	 * should run this tick.
	 */
	virtual bool IsAsyncEnabled() const { return true; }
};

}
