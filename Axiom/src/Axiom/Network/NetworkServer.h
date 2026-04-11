#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/IChainablePacket.h"

#include "Axiom/Environment/Entity/Player.h"
#include "Axiom/Environment/Level/Level.h"

#include <asio.hpp>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Axiom {

	class ChunkManager;

	class NetworkServer : public virtual RefCounted {
	public:
		NetworkServer() = default;
		~NetworkServer();

		NetworkServer(const NetworkServer&) = delete;
		NetworkServer& operator=(const NetworkServer&) = delete;

		void Start(uint16_t port);
		void Stop();

		void SetPacketHandler(Connection::PacketHandler handler) {
			m_PacketHandler = std::move(handler);
		}

		// ----- Players --------------------------------------------------

		Ref<Player> AddPlayer(Ref<Connection> connection,
			const std::string& name, const UUID& uuid);

		void RemovePlayer(ConnectionId connectionId);

		Ref<Player> GetPlayer(ConnectionId connectionId);

		Ref<Player> GetPlayer(const Ref<Connection>& connection) {
			return GetPlayer(connection->Id());
		}

		Ref<Player> GetPlayerByName(const std::string& name);
		Ref<Player> GetPlayerByUuid(const UUID& uuid);
		Ref<Player> GetPlayerByEntityId(int32_t entityId);

		std::vector<Ref<Player>> AllPlayers();
		int PlayerCount() const;

		// ----- Broadcasting ---------------------------------------------

		void BroadcastPacket(IChainablePacket& packet);
		void BroadcastPacketExcept(IChainablePacket& packet, ConnectionId exclude);

		void BroadcastPacketExcept(IChainablePacket& packet, const Ref<Connection>& exclude) {
			BroadcastPacketExcept(packet, exclude->Id());
		}

		// Broadcasts only to players whose chunk is within
		// min(source, observer) view distance of the source.
		// Source is always excluded.
		void BroadcastPacketNearby(const Player& source, IChainablePacket& packet);

		void SetChunkManager(ChunkManager* chunkManager) {
			m_ChunkManager = chunkManager;
		}

		// ----- Levels ---------------------------------------------------

		Ref<Level> GetLevel(const std::string& name) const;
		void AddLevel(const std::string& name, Ref<Level> level);

	private:
		void AcceptLoop();

		mutable std::shared_mutex m_PlayerMutex;
		std::unordered_map<ConnectionId, Ref<Player>> m_Players;

		mutable std::shared_mutex m_LevelMutex;
		std::unordered_map<std::string, Ref<Level>> m_Levels;

		Scope<asio::io_context> m_IoContext;
		Scope<asio::ip::tcp::acceptor> m_Acceptor;
		std::vector<std::thread> m_WorkerThreads;
		Connection::PacketHandler m_PacketHandler;
		std::atomic<bool> m_Running = false;

		ChunkManager* m_ChunkManager = nullptr;
	};

}
