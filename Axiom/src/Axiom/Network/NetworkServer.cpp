#include "NetworkServer.h"

#include "Axiom/Core/Log.h"

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

        if (m_Acceptor) {
            asio::error_code errorCode;
            m_Acceptor->close(errorCode);
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

        AX_CORE_INFO("Network server stopped");
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

                auto connection = CreateRef<Connection>(std::move(socket));
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

        auto player = CreateRef<Player>(std::move(connection), name, uuid);

        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        m_Players[player->GetConnection()->Id()] = player;

        AX_CORE_INFO("Player {} [{}] added", name, uuid.ToString());
        return player;
    }

    void NetworkServer::RemovePlayer(ConnectionId connectionId) {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        auto iterator = m_Players.find(connectionId);
        if (iterator != m_Players.end()) {
            AX_CORE_INFO("Player {} removed", iterator->second->Name());
            m_Players.erase(iterator);
        }
    }

    Ref<Player> NetworkServer::GetPlayer(ConnectionId connectionId) {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        auto iterator = m_Players.find(connectionId);
        if (iterator != m_Players.end()) {
            return iterator->second;
        }
        return nullptr;
    }

    Ref<Player> NetworkServer::GetPlayerByName(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        for (auto& [connectionId, player] : m_Players) {
            if (player->Name() == name) {
                return player;
            }
        }
        return nullptr;
    }

    Ref<Player> NetworkServer::GetPlayerByUuid(const UUID& uuid) {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        for (auto& [connectionId, player] : m_Players) {
            if (player->GetUuid() == uuid) {
                return player;
            }
        }
        return nullptr;
    }

    Ref<Player> NetworkServer::GetPlayerByEntityId(const int32_t entityId) {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        for (auto& player : m_Players | std::views::values) {
            if (player->GetEntityId() == entityId) {
                return player;
            }
        }
        return nullptr;
    }

    std::vector<Ref<Player>> NetworkServer::AllPlayers() {
        std::lock_guard<std::mutex> lock(m_PlayerMutex);
        std::vector<Ref<Player>> result;
        result.reserve(m_Players.size());
        for (auto& player : m_Players | std::views::values) {
            result.push_back(player);
        }
        return result;
    }

    int NetworkServer::PlayerCount() const {
        return static_cast<int>(m_Players.size());
    }

    // ----- Broadcasting ---------------------------------------------

    void NetworkServer::BroadcastPacket(IChainablePacket& packet) {
        NetworkBuffer payload;
        packet.Write(payload);

        for (const auto& player : AllPlayers()) {
            auto connection = player->GetConnection();
            if (connection && connection->IsConnected()
                && connection->State() == ConnectionState::Play) {
                connection->SendRawPacket(packet.GetPacketId(), payload);
            }
        }
    }

    void NetworkServer::BroadcastPacketExcept(IChainablePacket& packet, ConnectionId exclude) {
        NetworkBuffer payload;
        packet.Write(payload);

        for (const auto& player : AllPlayers()) {
            auto connection = player->GetConnection();
            if (connection && connection->IsConnected()
                && connection->State() == ConnectionState::Play
                && connection->Id() != exclude) {
                connection->SendRawPacket(packet.GetPacketId(), payload);
            }
        }
    }

    // ----- Levels ---------------------------------------------------

    Ref<Level> NetworkServer::GetLevel(const std::string& name) const {
        auto iterator = m_Levels.find(name);
        if (iterator != m_Levels.end()) {
            return iterator->second;
        }
        return nullptr;
    }

    void NetworkServer::AddLevel(const std::string& name, Ref<Level> level) {
        m_Levels[name] = std::move(level);
    }

}
