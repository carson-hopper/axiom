#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"

#include <asio.hpp>

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

namespace Axiom {

	class NetworkServer {
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

	private:
		void AcceptLoop();

		Scope<asio::io_context> m_IoContext;
		Scope<asio::ip::tcp::acceptor> m_Acceptor;
		std::vector<std::thread> m_WorkerThreads;
		Connection::PacketHandler m_PacketHandler;
		std::atomic<bool> m_Running = false;
	};

}
