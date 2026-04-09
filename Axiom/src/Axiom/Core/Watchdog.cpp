#include "axpch.h"
#include "Axiom/Core/Watchdog.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	Watchdog::Watchdog(int64_t maxTickTimeMillis)
		: m_MaxTickTimeMillis(maxTickTimeMillis) {
		m_LastTickStart.store(std::chrono::steady_clock::now());
	}

	Watchdog::~Watchdog() {
		Stop();
	}

	void Watchdog::TickStarted() {
		m_LastTickStart.store(std::chrono::steady_clock::now());
		m_InTick.store(true);
	}

	void Watchdog::TickFinished() {
		m_InTick.store(false);
	}

	void Watchdog::Start() {
		if (m_Running.exchange(true)) {
			return;
		}

		m_Thread = std::thread([this] {
			MonitorLoop();
		});

		AX_CORE_INFO("Watchdog started (max tick time: {}ms)", m_MaxTickTimeMillis);
	}

	void Watchdog::Stop() {
		if (!m_Running.exchange(false)) {
			return;
		}

		m_ConditionVariable.notify_all();

		if (m_Thread.joinable()) {
			m_Thread.join();
		}

		AX_CORE_INFO("Watchdog stopped");
	}

	void Watchdog::MonitorLoop() {
		while (m_Running) {
			{
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_ConditionVariable.wait_for(lock, std::chrono::seconds(1), [this] {
					return !m_Running.load();
				});
			}

			if (!m_Running) {
				break;
			}

			if (!m_InTick.load()) {
				continue;
			}

			auto tickStart = m_LastTickStart.load();
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
				now - tickStart).count();

			if (elapsed > m_MaxTickTimeMillis) {
				AX_CORE_WARN("Watchdog: tick has been running for {}ms (limit: {}ms)",
					elapsed, m_MaxTickTimeMillis);
			}
		}
	}

}
