#include "axpch.h"
#include "Axiom/Core/TickStack.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	TickStack::TickStack() {
		// Create worker threads (hardware concurrency - 1, leaving 1 for main thread)
		unsigned int threadCount = std::max(1u, std::thread::hardware_concurrency() - 1);
		
		for (unsigned int i = 0; i < threadCount; i++) {
			m_WorkerThreads.emplace_back([this]() {
				WorkerThreadLoop();
			});
		}

		AX_CORE_INFO("TickStack created with {} worker threads", threadCount);
	}

	TickStack::~TickStack() {
		Stop();
		
		// Signal workers to stop
		m_StopWorkers = true;
		m_TaskCV.notify_all();

		// Wait for all workers to finish
		for (auto& thread : m_WorkerThreads) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		// Process any remaining main thread tasks
		ProcessMainThreadTasks();

		// Notify remaining tickables
		for (Tickable* tick : m_Tickables) {
			if (tick) {
				tick->OnTickUnregistered();
			}
		}
	}

	void TickStack::PushTick(Tickable* tickable) {
		std::lock_guard<std::mutex> lock(m_TickablesMutex);
		
		auto it = std::find(m_Tickables.begin(), m_Tickables.end(), tickable);
		if (it == m_Tickables.end()) {
			m_Tickables.push_back(tickable);
			m_TickablesDirty = true;
			tickable->OnTickRegistered();
		}
	}

	void TickStack::PopTick(Tickable* tickable) {
		std::lock_guard<std::mutex> lock(m_TickablesMutex);
		
		auto it = std::find(m_Tickables.begin(), m_Tickables.end(), tickable);
		if (it != m_Tickables.end()) {
			tickable->OnTickUnregistered();
			m_Tickables.erase(it);
			m_TickablesDirty = true;
		}
	}

	void TickStack::PostToMainThread(std::function<void()> task) {
		{
			std::lock_guard<std::mutex> lock(m_MainThreadMutex);
			m_MainThreadTasks.push(std::move(task));
		}
	}

	void TickStack::RunSyncLoop(std::function<bool()> shouldContinue, float targetTPS) {
		m_Running = true;
		m_TargetDeltaTime = 1.0f / targetTPS;
		
		AX_CORE_INFO("TickStack started at {:.1f} TPS ({}ms per tick)", targetTPS, m_TargetDeltaTime * 1000.0f);

		auto lastTickTime = std::chrono::steady_clock::now();
		m_LastTPSCalcTime = lastTickTime;
		m_TickCountSinceLastCalc = 0;

		while (m_Running && shouldContinue()) {
			auto currentTime = std::chrono::steady_clock::now();
			
			// Calculate elapsed time
			float deltaTime = std::chrono::duration<float>(currentTime - lastTickTime).count();
			
			// If enough time has passed, execute a tick
			if (deltaTime >= m_TargetDeltaTime) {
				lastTickTime = currentTime;
				
				Tick(Timestep(deltaTime));
				
				// Calculate TPS
				m_TickCountSinceLastCalc++;
				auto timeSinceLastCalc = std::chrono::duration<float>(currentTime - m_LastTPSCalcTime).count();
				if (timeSinceLastCalc >= 1.0f) {
					m_ActualTPS.store(static_cast<float>(m_TickCountSinceLastCalc) / timeSinceLastCalc);
					m_TickCountSinceLastCalc = 0;
					m_LastTPSCalcTime = currentTime;
				}
			} else {
				// Small sleep to prevent busy-waiting
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		m_Running = false;
		AX_CORE_INFO("TickStack stopped. Final TPS: {:.2f}", m_ActualTPS.load());
	}

	void TickStack::Tick(Timestep timestep) {
		m_CurrentTick++;

		// Sort tickables by phase if needed
		if (m_TickablesDirty) {
			SortTickablesByPhase();
			m_TickablesDirty = false;
		}

		// Process main thread tasks (from async operations)
		ProcessMainThreadTasks();

		// Execute tickables
		std::vector<Tickable*> tickablesCopy;
		{
			std::lock_guard<std::mutex> lock(m_TickablesMutex);
			tickablesCopy = m_Tickables;
		}

		for (auto* tickable : tickablesCopy) {
			if (tickable && tickable->IsTickEnabled()) {
				tickable->OnTick(timestep);
			}
		}
	}

	void TickStack::ProcessMainThreadTasks() {
		std::queue<std::function<void()>> tasks;
		
		{
			std::lock_guard<std::mutex> lock(m_MainThreadMutex);
			tasks.swap(m_MainThreadTasks);
		}

		while (!tasks.empty()) {
			tasks.front()();
			tasks.pop();
		}
	}

	void TickStack::WorkerThreadLoop() {
		while (!m_StopWorkers) {
			std::function<void()> task;
			
			{
				std::unique_lock<std::mutex> lock(m_TaskMutex);
				m_TaskCV.wait(lock, [this] { return !m_AsyncTasks.empty() || m_StopWorkers; });
				
				if (m_StopWorkers) break;
				
				task = std::move(m_AsyncTasks.front());
				m_AsyncTasks.pop();
			}
			
			if (task) {
				task();
			}
		}
	}

	void TickStack::SortTickablesByPhase() {
		std::lock_guard<std::mutex> lock(m_TickablesMutex);
		
		std::sort(m_Tickables.begin(), m_Tickables.end(), [](Tickable* a, Tickable* b) {
			return a->GetTickPhase() < b->GetTickPhase();
		});
	}

}
