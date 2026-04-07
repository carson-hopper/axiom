#include "PlayerManager.h"

#include "Axiom/Core/Log.h"

#include <ranges>

namespace Axiom {

	Ref<Player> PlayerManager::AddPlayer(const int32_t entityId, Ref<Connection> connection,
		const std::string& name, const std::string& uuid) {

		auto player = CreateRef<Player>(entityId, std::move(connection), name, uuid);

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_PlayersByConnection[player->GetConnection().get()] = player;

		AX_CORE_INFO("Player {} [{}] added (entity {})", name, uuid, entityId);
		return player;
	}

	void PlayerManager::RemovePlayer(Connection* connection) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto iterator = m_PlayersByConnection.find(connection);
		if (iterator != m_PlayersByConnection.end()) {
			AX_CORE_INFO("Player {} removed", iterator->second->Name());
			m_PlayersByConnection.erase(iterator);
		}
	}

	Ref<Player> PlayerManager::GetPlayer(Connection* connection) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto iterator = m_PlayersByConnection.find(connection);
		if (iterator != m_PlayersByConnection.end()) {
			return iterator->second;
		}
		return nullptr;
	}

	Ref<Player> PlayerManager::GetPlayerByName(const std::string& name) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (auto& [connection, player] : m_PlayersByConnection) {
			if (player->Name() == name) {
				return player;
			}
		}
		return nullptr;
	}

	Ref<Player> PlayerManager::GetPlayerByUuid(const std::string& uuid) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (auto& [connection, player] : m_PlayersByConnection) {
			if (player->Uuid() == uuid) {
				return player;
			}
		}
		return nullptr;
	}

	Ref<Player> PlayerManager::GetPlayerByEntityId(const int32_t entityId) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (auto& player : m_PlayersByConnection | std::views::values) {
			if (player->GetEntityId() == entityId) {
				return player;
			}
		}
		return nullptr;
	}

	std::vector<Ref<Player>> PlayerManager::AllPlayers() {
		std::lock_guard<std::mutex> lock(m_Mutex);
		std::vector<Ref<Player>> result;
		result.reserve(m_PlayersByConnection.size());
		for (auto& player : m_PlayersByConnection | std::views::values) {
			result.push_back(player);
		}
		return result;
	}

	int PlayerManager::PlayerCount() const {
		return static_cast<int>(m_PlayersByConnection.size());
	}

}
