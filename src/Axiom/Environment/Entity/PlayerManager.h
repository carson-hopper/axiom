#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Entity/Player.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	class Connection;

	/**
	 * Tracks all connected players on the server.
	 * Provides lookup by entity ID, name, UUID, or connection.
	 */
	class PlayerManager {
	public:
		Ref<Player> AddPlayer(int32_t entityId, Ref<Connection> connection,
			const std::string& name, const std::string& uuid);

		void RemovePlayer(Connection* connection);

		Ref<Player> GetPlayer(Connection* connection);
		Ref<Player> GetPlayerByName(const std::string& name);
		Ref<Player> GetPlayerByUuid(const std::string& uuid);
		Ref<Player> GetPlayerByEntityId(int32_t entityId);

		std::vector<Ref<Player>> AllPlayers();
		int PlayerCount() const;

		int32_t NextEntityId() { return m_NextEntityId++; }

	private:
		std::mutex m_Mutex;
		std::unordered_map<Connection*, Ref<Player>> m_PlayersByConnection;
		std::atomic<int32_t> m_NextEntityId{1};
	};

}
