#include "axpch.h"
#include "Axiom/Core/ExampleAsyncSystem.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Application.h"

namespace Axiom {

	ExampleAsyncSystem::ExampleAsyncSystem()
		: m_Enabled(true) {
	}

	void ExampleAsyncSystem::OnTickRegistered() {
		AX_CORE_INFO("ExampleAsyncSystem registered for ticking");
	}

	void ExampleAsyncSystem::OnTickUnregistered() {
		AX_CORE_INFO("ExampleAsyncSystem unregistered from ticking");
	}

	void ExampleAsyncSystem::OnTick(Timestep timestep) {
		// Accumulate time
		m_TimeAccumulator += timestep.GetSeconds();

		// Every 2 seconds, do some async work
		if (m_TimeAccumulator >= 2.0f) {
			m_TimeAccumulator = 0.0f;
			DoAsyncWork();
		}

		// Check if we have results from async work
		if (m_HasResult.load()) {
			AX_CORE_INFO("Async result received: {}", m_AsyncResult.load());
			m_HasResult = false;
		}
	}

	void ExampleAsyncSystem::DoAsyncWork() {
		// Get reference to the tick stack for async operations
		TickStack& tickStack = Application::Instance().GetTickStack();

		// Execute heavy work asynchronously
		auto future = tickStack.Async([this]() -> int {
			// Simulate heavy computation
			AX_CORE_TRACE("Starting async work on background thread...");
			
			int result = 0;
			for (int i = 0; i < 1000000; i++) {
				result += i;
			}
			
			AX_CORE_TRACE("Async work completed, result: {}", result);
			return result;
		});

		// We could wait for the future, but that would block.
		// Instead, we'll poll for the result in OnTick.
		// For a more elegant solution, use PostToMainThread:

		/* Alternative approach using PostToMainThread:
		tickStack.Async([this, &tickStack]() {
			int result = HeavyComputation();
			
			tickStack.PostToMainThread([this, result]() {
				m_AsyncResult = result;
				m_HasResult = true;
			});
		});
		*/
	}

}
