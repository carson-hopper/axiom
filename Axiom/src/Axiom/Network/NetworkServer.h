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

		/**
		 * Broadcast `packet` only to players whose chunk is
		 * within `min(source, observer)` view distance of
		 * `source`'s chunk — i.e. both players must have
		 * picked a render radius that covers the other. The
		 * source itself is always excluded; callers that
		 * need an echo should use `BroadcastPacket` or
		 * `BroadcastPacketExcept`.
		 *
		 * Typical callers: block-break / block-place, entity
		 * movement, particles, sounds, break progress. These
		 * events are only meaningful to players who have the
		 * affected chunk loaded, and the `min(a, b)` radius
		 * test is a cheap, symmetric way to approximate
		 * "both players have each other's chunk loaded".
		 *
		 * Requires `SetChunkManager` to have been called —
		 * without the chunk manager there's no per-player
		 * view distance to test against, and the call
		 * falls back to the full broadcast so events still
		 * reach everyone (logged at warn level).
		 */
		void BroadcastPacketNearby(const Player& source, IChainablePacket& packet);

		/**
		 * Install the chunk manager pointer this server uses
		 * to look up per-player view distances for
		 * `BroadcastPacketNearby`. Non-owning — the chunk
		 * manager must outlive the network server. Called
		 * once during `PacketContext` construction.
		 */
		void SetChunkManager(ChunkManager* chunkManager) {
			m_ChunkManager = chunkManager;
		}

		// ----- Levels ---------------------------------------------------

		Ref<Level> GetLevel(const std::string& name) const;
		void AddLevel(const std::string& name, Ref<Level> level);

	private:
		void AcceptLoop();

		/**
		 * Shared mutex: reads (GetPlayer*, AllPlayers,
		 * PlayerCount, broadcast iteration via AllPlayers)
		 * use std::shared_lock so many callers can run
		 * concurrently; writes (AddPlayer, RemovePlayer)
		 * use std::lock_guard for exclusive access. Mutable
		 * so const accessors (PlayerCount) can still lock.
		 */
		mutable std::shared_mutex m_PlayerMutex;
		std::unordered_map<ConnectionId, Ref<Player>> m_Players;

		/**
		 * Separate mutex for the level registry. Levels are
		 * added infrequently (world load) and read often
		 * (dimension lookup), so a shared_mutex is the right
		 * fit here too.
		 */
		mutable std::shared_mutex m_LevelMutex;
		std::unordered_map<std::string, Ref<Level>> m_Levels;

		Scope<asio::io_context> m_IoContext;
		Scope<asio::ip::tcp::acceptor> m_Acceptor;
		std::vector<std::thread> m_WorkerThreads;
		Connection::PacketHandler m_PacketHandler;
		std::atomic<bool> m_Running = false;

		/**
		 * Non-owning pointer to the chunk manager.
		 * Used only by `BroadcastPacketNearby` to snapshot
		 * per-player view distances. `nullptr` until
		 * `PacketContext` calls `SetChunkManager` during
		 * its constructor; `BroadcastPacketNearby` degrades
		 * to a full broadcast (with a warning) when it's
		 * still null.
		 */
		ChunkManager* m_ChunkManager = nullptr;
	};

}
