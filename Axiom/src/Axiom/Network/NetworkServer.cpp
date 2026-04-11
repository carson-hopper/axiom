#include "NetworkServer.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/Level/ChunkManager.h"

#include <algorithm>
#include <ranges>
#include <thread>

namespace Axiom {

	NetworkServer::~NetworkServer() {
		Stop();
	}

	void NetworkServer::Start(uint16_t port) {
		m_IoContext = CreateScope<asio::io_context>();

		auto endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port);
		m_Acceptor = CreateScope<asio::ip::tcp::acceptor>(*m_IoContext, endpoint);
		m_Acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));

		m_Running = true;
		AX_CORE_INFO("Network server listening on port {}", port);

		AcceptLoop();

		// Run the io_context on worker threads
		unsigned int threadCount = std::max(1u, std::thread::hardware_concurrency() / 2);
		for (unsigned int i = 0; i < threadCount; i++) {
			m_WorkerThreads.emplace_back([this]() {
				m_IoContext->run();
			});
		}
	}

	void NetworkServer::Stop() {
		if (!m_Running) {
			return;
		}
		m_Running = false;

		// Stop accepting new connections first so we do
		// not pick up a brand-new client mid-shutdown.
		if (m_Acceptor) {
			asio::error_code errorCode;
			m_Acceptor->close(errorCode);
		}

		// Gracefully disconnect every connected player
		// BEFORE yanking the io_context. Each Disconnect
		// call logs "Disconnecting ...: Server shutting
		// down", cancels the read timer, drains the
		// write queue, flips m_Connected to false, and
		// posts a socket shutdown onto the io_context.
		// This gives connections a clean server-side
		// teardown instead of the abrupt cancellation
		// that `io_context::stop()` alone would cause.
		//
		// AllPlayers() returns a snapshot vector (built
		// under the shared lock) so we are not iterating
		// the player map while any concurrent
		// RemovePlayer runs through it.
		const auto players = AllPlayers();
		for (const auto& player : players) {
			auto connection = player->GetConnection();
			if (connection && connection->IsConnected()) {
				connection->Disconnect("Server shutting down");
			}
		}

		if (m_IoContext) {
			m_IoContext->stop();
		}

		for (auto& thread : m_WorkerThreads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
		m_WorkerThreads.clear();

		AX_CORE_INFO("Network server stopped ({} players disconnected)", players.size());
	}

	void NetworkServer::AcceptLoop() {
		m_Acceptor->async_accept(
			[this](const asio::error_code& errorCode, asio::ip::tcp::socket socket) {
				if (errorCode) {
					if (m_Running) {
						AX_CORE_ERROR("Accept error: {}", errorCode.message());
					}
					return;
				}

				auto connection = Ref<Connection>::Create(std::move(socket));
				if (m_PacketHandler) {
					connection->SetPacketHandler(m_PacketHandler);
				}
				connection->Start();

				if (m_Running) {
					AcceptLoop();
				}
			});
	}

	// ----- Players --------------------------------------------------

	Ref<Player> NetworkServer::AddPlayer(Ref<Connection> connection,
		const std::string& name, const UUID& uuid) {

		auto player = Ref<Player>::Create(std::move(connection), name, uuid);

		// Exclusive lock: writer.
		std::lock_guard<std::shared_mutex> const lock(m_PlayerMutex);
		m_Players[player->GetConnection()->Id()] = player;

		AX_CORE_INFO("Player {} [{}] added", name, uuid.ToString());
		return player;
	}

	void NetworkServer::RemovePlayer(ConnectionId connectionId) {
		// Exclusive lock: writer.
		std::lock_guard<std::shared_mutex> const lock(m_PlayerMutex);
		auto iterator = m_Players.find(connectionId);
		if (iterator != m_Players.end()) {
			AX_CORE_INFO("Player {} removed", iterator->second->Name());
			m_Players.erase(iterator);
		}
	}

	Ref<Player> NetworkServer::GetPlayer(ConnectionId connectionId) {
		// Shared lock: reader. Multiple lookups can run
		// concurrently without blocking each other.
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		auto iterator = m_Players.find(connectionId);
		if (iterator != m_Players.end()) {
			return iterator->second;
		}
		return nullptr;
	}

	Ref<Player> NetworkServer::GetPlayerByName(const std::string& name) {
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		for (auto& [connectionId, player] : m_Players) {
			if (player->Name() == name) {
				return player;
			}
		}
		return nullptr;
	}

	Ref<Player> NetworkServer::GetPlayerByUuid(const UUID& uuid) {
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		for (auto& [connectionId, player] : m_Players) {
			if (player->GetUuid() == uuid) {
				return player;
			}
		}
		return nullptr;
	}

	Ref<Player> NetworkServer::GetPlayerByEntityId(const int32_t entityId) {
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		for (auto& player : m_Players | std::views::values) {
			if (player->GetEntityId() == entityId) {
				return player;
			}
		}
		return nullptr;
	}

	std::vector<Ref<Player>> NetworkServer::AllPlayers() {
		// Build the snapshot under the shared lock. The
		// returned vector is by-value so callers can
		// iterate without holding any lock.
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		std::vector<Ref<Player>> result;
		result.reserve(m_Players.size());
		for (auto& player : m_Players | std::views::values) {
			result.push_back(player);
		}
		return result;
	}

	int NetworkServer::PlayerCount() const {
		// Was previously unlocked — racy against any
		// concurrent AddPlayer / RemovePlayer. Now a
		// shared read.
		std::shared_lock<std::shared_mutex> const lock(m_PlayerMutex);
		return static_cast<int>(m_Players.size());
	}

	// ----- Broadcasting ---------------------------------------------

	void NetworkServer::BroadcastPacket(IChainablePacket& packet) {
		for (const auto& player : AllPlayers()) {
			auto connection = player->GetConnection();
			if (connection && connection->IsConnected()
				&& connection->State() == ConnectionState::Play) {
				connection->SendRawPacket(packet);
			}
		}
	}

	void NetworkServer::BroadcastPacketExcept(IChainablePacket& packet, const ConnectionId exclude) {
		for (const auto& player : AllPlayers()) {
			auto connection = player->GetConnection();
			if (connection && connection->IsConnected()
				&& connection->State() == ConnectionState::Play
				&& connection->Id() != exclude) {
				connection->SendRawPacket(packet);
			}
		}
	}

	void NetworkServer::BroadcastPacketNearby(const Player& source, IChainablePacket& packet) {
		if (!m_ChunkManager) {
			AX_CORE_WARN(
				"BroadcastPacketNearby called before SetChunkManager; "
				"falling back to full broadcast");
			BroadcastPacketExcept(packet, source.GetConnection()->Id());
			return;
		}

		const auto viewDistances = m_ChunkManager->SnapshotViewDistances();

		const auto sourceConnection = source.GetConnection();
		if (!sourceConnection) {
			return;
		}
		const ConnectionId sourceId = sourceConnection->Id();
		const auto sourceIterator = viewDistances.find(sourceId);
		if (sourceIterator == viewDistances.end()) {
			return;
		}
		const int sourceViewDistance = sourceIterator->second;
		const int32_t sourceChunkX = source.GetPosition().ChunkX();
		const int32_t sourceChunkZ = source.GetPosition().ChunkZ();

		NetworkBuffer payload;
		packet.Write(payload);
		const int32_t packetId = packet.GetPacketId();

		for (const auto& player : AllPlayers()) {
			if (!player) {
				continue;
			}
			const auto connection = player->GetConnection();
			if (!connection || !connection->IsConnected()
				|| connection->State() != ConnectionState::Play) {
				continue;
			}
			const ConnectionId observerId = connection->Id();
			if (observerId == sourceId) {
				continue; // never echo to the source
			}

			const auto observerIterator = viewDistances.find(observerId);
			if (observerIterator == viewDistances.end()) {
				continue;
			}
			const int observerViewDistance = observerIterator->second;
			const int effectiveRadius =
				std::min(sourceViewDistance, observerViewDistance);

			const int32_t observerChunkX = player->GetPosition().ChunkX();
			const int32_t observerChunkZ = player->GetPosition().ChunkZ();
			const int chunkDeltaX = std::abs(sourceChunkX - observerChunkX);
			const int chunkDeltaZ = std::abs(sourceChunkZ - observerChunkZ);
			if (std::max(chunkDeltaX, chunkDeltaZ) > effectiveRadius) {
				continue;
			}

			connection->SendRawPacket(packetId, payload);
		}
	}

	// ----- Levels ---------------------------------------------------

	Ref<Level> NetworkServer::GetLevel(const std::string& name) const {
		std::shared_lock<std::shared_mutex> const lock(m_LevelMutex);
		const auto iterator = m_Levels.find(name);
		if (iterator != m_Levels.end()) {
			return iterator->second;
		}
		return nullptr;
	}

	void NetworkServer::AddLevel(const std::string& name, Ref<Level> level) {
		std::lock_guard<std::shared_mutex> const lock(m_LevelMutex);
		m_Levels[name] = std::move(level);
	}

}
