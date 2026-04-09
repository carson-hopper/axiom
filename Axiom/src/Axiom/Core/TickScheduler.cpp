#include "axpch.h"
#include "Axiom/Core/TickScheduler.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Time.h"

namespace Axiom {

	TickScheduler::TickScheduler(TickRegistry& registry, TaskQueue& taskQueue, AsyncExecutor& executor)
		: m_Registry(registry)
		, m_TaskQueue(taskQueue)
		, m_Executor(executor) {
	}

	void TickScheduler::RunSyncLoop(const std::function<bool()>& shouldContinue, float targetTPS) {
		m_Running = true;
		const float targetDeltaTime = 1.0f / targetTPS;
		float lastTickTime = Time::GetTime();

		AX_CORE_INFO("TickScheduler started at {:.1f} TPS ({}ms per tick)",
			targetTPS, targetDeltaTime * 1000.0f);

		m_LastTPSCalcTime = std::chrono::steady_clock::now();
		m_TickCountSinceLastCalc = 0;

		while (m_Running && shouldContinue()) {
			const float currentTime = Time::GetTime();
			const Timestep timestep = currentTime - lastTickTime;

			if (timestep.GetSeconds() < targetDeltaTime) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			lastTickTime = currentTime;
			Tick(timestep);

			m_TickCountSinceLastCalc++;
			const auto now = std::chrono::steady_clock::now();
			const float timeSinceLastCalc = std::chrono::duration<float>(
				now - m_LastTPSCalcTime).count();

			if (timeSinceLastCalc >= 1.0f) {
				m_ActualTPS.store(
					static_cast<float>(m_TickCountSinceLastCalc) / timeSinceLastCalc);
				m_TickCountSinceLastCalc = 0;
				m_LastTPSCalcTime = now;
			}
		}

		m_Running = false;
		AX_CORE_INFO("TickScheduler stopped. Final TPS: {:.2f}", m_ActualTPS.load());
	}

	void TickScheduler::Tick(Timestep timestep) {
		++m_CurrentTick;

		// Phase 1: Main-thread synchronous tickables (sorted by phase)
		auto tickables = m_Registry.GetTickables();
		for (auto& tickable : tickables) {
			tickable->OnTick(timestep);
		}

		// Phase 2: Prepare async tickables on main thread
		auto asyncTickables = m_Registry.GetAsyncTickables();
		for (auto& asyncTickable : asyncTickables) {
			asyncTickable->PrepareAsync(timestep);
		}

		// Phase 3: Dispatch TickAsync() to background executor
		// Phase 4: Process TaskQueue on main thread while async work runs
		// Phase 5: Wait for async work to complete
		if (!asyncTickables.empty()) {
			/**
			 * Submit all async work first, then process the
			 * task queue while the executor works. The
			 * executor blocks until completion.
			 */
			auto asyncFuture = m_Executor.Submit([this, &asyncTickables]() {
				m_Executor.DispatchAndWait(asyncTickables);
			});

			m_TaskQueue.ProcessAll();
			asyncFuture.get();
		} else {
			m_TaskQueue.ProcessAll();
		}

		// Phase 6: Merge async results on main thread
		for (auto& asyncTickable : asyncTickables) {
			asyncTickable->MergeResults();
		}
	}

}
