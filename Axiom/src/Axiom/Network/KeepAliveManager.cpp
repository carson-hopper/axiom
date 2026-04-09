#include "KeepAliveManager.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	void KeepAliveManager::Start() {
		m_Running = true;
		m_Thread = std::thread(&KeepAliveManager::TickLoop, this);
	}

	void KeepAliveManager::Stop() {
		m_Running = false;
		if (m_Thread.joinable()) {
			m_Thread.join();
		}
	}

	void KeepAliveManager::OnKeepAliveResponse(ConnectionId connectionId, const int64_t keepAliveId) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto iterator = m_Pending.find(connectionId);
		if (iterator != m_Pending.end() && iterator->second.keepAliveId == keepAliveId) {
			iterator->second.responded = true;
		}
	}

	void KeepAliveManager::TickLoop() {
		while (m_Running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(KeepAliveIntervalMs));
			if (!m_Running) break;

			CheckTimeouts();
			SendKeepAlives();
		}
	}

	void KeepAliveManager::SendKeepAlives() {
		int64_t keepAliveId = m_NextKeepAliveId++;
		int64_t now = NowMillis();

		auto players = m_PlayerManager.AllPlayers();

		std::lock_guard<std::mutex> lock(m_Mutex);
		for (const auto& player : players) {
			auto connection = player->GetConnection();
			if (!connection || !connection->IsConnected()) continue;
			if (connection->State() != ConnectionState::Play) continue;

			NetworkBuffer payload;
			payload.WriteLong(keepAliveId);
			connection->SendRawPacket(Clientbound::Play::KeepAlive, payload);

			m_Pending[connection->Id()] = PendingKeepAlive{keepAliveId, now, false};
		}
	}

	void KeepAliveManager::CheckTimeouts() {
		int64_t now = NowMillis();

		std::vector<ConnectionId> timedOut;

		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			for (auto& [connectionId, pending] : m_Pending) {
				if (!pending.responded && (now - pending.sentAtMs) > KeepAliveTimeoutMs) {
					timedOut.push_back(connectionId);
				}
			}

			for (const auto& connectionId : timedOut) {
				m_Pending.erase(connectionId);
			}
		}

		for (const auto& connectionId : timedOut) {
			auto player = m_PlayerManager.GetPlayer(connectionId);
			if (player) {
				auto connection = player->GetConnection();
				if (connection) {
					connection->Disconnect("Timed out");
				}
			}
			m_PlayerManager.RemovePlayer(connectionId);
		}
	}

}
