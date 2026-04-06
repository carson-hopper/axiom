#include "NetworkServer.h"

#include "Axiom/Core/Log.h"

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

}
