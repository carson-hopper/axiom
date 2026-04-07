#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Tickable.h"

namespace Axiom {

/**
 * Example system demonstrating TickStack with Async.
 *
 * Shows how to:
 * 1. Extend Tickable for 20 TPS updates
 * 2. Use Async() for background work
 * 3. Use PostToMainThread() to apply results
 */
class ExampleAsyncSystem : public Tickable {
public:
	ExampleAsyncSystem();
	~ExampleAsyncSystem() override = default;

	// Called when registered to tick stack
	void OnTickRegistered() override;

	// Called every tick (50ms at 20 TPS)
	void OnTick(Timestep timestep) override;

	// Called when unregistered
	void OnTickUnregistered() override;

	// Control ticking
	bool IsTickEnabled() const override { return m_Enabled; }
	void SetEnabled(bool enabled) { m_Enabled = enabled; }

	// Lower phase = earlier in tick order
	uint32_t GetTickPhase() const override { return 100; }

private:
	void DoAsyncWork();

private:
	bool m_Enabled = true;
	float m_TimeAccumulator = 0.0f;
	
	// Results from async work
	std::atomic<int> m_AsyncResult{0};
	std::atomic<bool> m_HasResult{false};
};

}
