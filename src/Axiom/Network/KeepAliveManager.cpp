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

	void KeepAliveManager::OnKeepAliveResponse(Connection* connection, const int64_t keepAliveId) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto iterator = m_Pending.find(connection);
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

			m_Pending[connection.get()] = PendingKeepAlive{keepAliveId, now, false};
		}
	}

	void KeepAliveManager::CheckTimeouts() {
		int64_t now = NowMillis();

		std::vector<Connection*> timedOut;

		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			for (auto& [connection, pending] : m_Pending) {
				if (!pending.responded && (now - pending.sentAtMs) > KeepAliveTimeoutMs) {
					timedOut.push_back(connection);
				}
			}

			for (auto* connection : timedOut) {
				m_Pending.erase(connection);
			}
		}

		for (auto* connection : timedOut) {
			connection->Disconnect("Timed out");
			m_PlayerManager.RemovePlayer(connection);
		}
	}

}
