#pragma once

#include "Axiom/Core/Timestep.h"

namespace Axiom {

/**
 * Interface for objects that need to be updated on the game tick.
 *
 * Minecraft runs at 20 TPS (50ms per tick). Classes implementing Tickable
 * receive OnTick() calls synchronously on the main thread, but can perform
 * async work using coroutines or the AsyncTask system.
 *
 * Example:
 * @code
 * class MySystem : public Tickable {
 * public:
 *     void OnTick(Timestep timestep) override {
 *         // Synchronous work (main thread)
 *         ProcessLogic();
 *
 *         // Async work (background thread)
 *         Async([this]() {
 *             HeavyComputation();
 *             PostToMainThread([this]() {
 *                 ApplyResults();
 *             });
 *         });
 *     }
 * };
 * @endcode
 */
class Tickable {
public:
	virtual ~Tickable() = default;

	/**
	 * Called every game tick (50ms interval, 20 TPS).
	 *
	 * This runs on the main thread. For heavy work, use Async() to offload
	 * to background threads, then PostToMainThread() to apply results.
	 *
	 * @param timestep Time since last tick (typically 0.05s, but can vary)
	 */
	virtual void OnTick(Timestep timestep) = 0;

	/**
	 * Called when the tickable is registered to the TickManager.
	 * Use for initialization that requires the tick system.
	 */
	virtual void OnTickRegistered() {}

	/**
	 * Called when the tickable is unregistered from the TickManager.
	 * Use for cleanup of tick-related resources.
	 */
	virtual void OnTickUnregistered() {}

	/**
	 * Check if this tickable should receive OnTick calls.
	 * Return false to pause ticking without unregistering.
	 */
	virtual bool IsTickEnabled() const { return true; }

	/**
	 * Get the desired tick phase. Lower phases tick first.
	 * Use for ordering dependencies between tickables.
	 */
	virtual uint32_t GetTickPhase() const { return 100; }
};

} // namespace Axiom
