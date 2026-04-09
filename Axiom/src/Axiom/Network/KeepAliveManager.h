#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace Axiom {

	/**
	 * Periodically sends keep-alive packets to all connected players
	 * and disconnects those who don't respond within the timeout.
	 */
	class KeepAliveManager {
	public:
		static constexpr int64_t KeepAliveIntervalMs = 15000;  // 15 seconds
		static constexpr int64_t KeepAliveTimeoutMs = 30000;   // 30 seconds

		explicit KeepAliveManager(class NetworkServer& server)
			: m_Server(server) {}

		~KeepAliveManager() { Stop(); }

		KeepAliveManager(const KeepAliveManager&) = delete;
		KeepAliveManager& operator=(const KeepAliveManager&) = delete;

		void Start();
		void Stop();

		/**
		 * Called when a keep-alive response is received from a player.
		 */
		void OnKeepAliveResponse(ConnectionId connectionId, int64_t keepAliveId);

	private:
		void TickLoop();
		void SendKeepAlives();
		void CheckTimeouts();

		static int64_t NowMillis() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now().time_since_epoch()).count();
		}

		struct PendingKeepAlive {
			int64_t keepAliveId = 0;
			int64_t sentAtMs = 0;
			bool responded = false;
		};

		NetworkServer& m_Server;

		std::mutex m_Mutex;
		std::unordered_map<ConnectionId, PendingKeepAlive> m_Pending;

		std::thread m_Thread;
		std::atomic<bool> m_Running = false;
		int64_t m_NextKeepAliveId = 1;
	};

}
