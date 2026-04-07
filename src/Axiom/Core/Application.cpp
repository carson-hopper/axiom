#include "Application.h"

#include "Axiom/Core/Assert.h"
#include "Axiom/Event/ServerEvents.h"
#include "Axiom/Plugin/CorePlugin.h"
#include "Axiom/Command/CommandSender.h"

#include "Axiom/Network/Packet/26.1/Handshake/HandshakePacket.h"
#include "Axiom/Network/Packet/26.1/Status/StatusRequestPacket.h"
#include "Axiom/Network/Packet/26.1/Status/PingRequestPacket.h"
#include "Axiom/Network/Packet/26.1/Login/LoginHelloPacket.h"
#include "Axiom/Network/Packet/26.1/Login/EncryptionResponsePacket.h"
#include "Axiom/Network/Packet/26.1/Login/LoginAcknowledgedPacket.h"

#include <iostream>
#include <string>

namespace Axiom {

	Application* Application::s_Instance = nullptr;

	void Application::Init() {
		AX_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		Log::Init();
		AX_CORE_INFO("Axiom Server v0.1.0 starting...");

		m_Config = CreateScope<ServerConfig>();
		m_Config->Load("server.yml");

		m_EventBus = CreateScope<EventBus>();
		m_CommandRegistry = CreateScope<CommandRegistry>();
		m_PluginManager = CreateScope<PluginManager>();

		m_PluginContext = CreateScope<PluginContext>(*m_EventBus, *m_CommandRegistry, *m_Config);

		m_PluginManager->RegisterPlugin(CreateScope<CorePlugin>());
		m_PluginManager->EnableAll(*m_PluginContext);

		m_PacketContext = CreateScope<PacketContext>(*m_Config, *m_EventBus, *m_CommandRegistry);
		RegisterPackets();

		m_NetworkServer = CreateScope<NetworkServer>();
		m_NetworkServer->SetPacketHandler(
			[this](Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer) {
				m_PacketRegistry.Dispatch(connection->State(), packetId, buffer,
					std::move(connection), *m_PacketContext);
			});
		m_NetworkServer->Start(m_Config->Port());

		AX_CORE_INFO("Server initialized on port {}", m_Config->Port());
	}

	void Application::RegisterPackets() {
		// Handshake
		m_PacketRegistry.Register<775, HandshakePacket<775>>();

		// Status
		m_PacketRegistry.Register<775, StatusRequestPacket<775>>();
		m_PacketRegistry.Register<775, PingRequestPacket<775>>();

		// Login
		m_PacketRegistry.Register<775, LoginHelloPacket<775>>();
		m_PacketRegistry.Register<775, EncryptionResponsePacket<775>>();
		m_PacketRegistry.Register<775, LoginAcknowledgedPacket<775>>();
	}

	void Application::Run() {
		m_Running = true;

		ServerStartEvent startEvent;
		m_EventBus->Publish(startEvent);

		AX_CORE_INFO("Server is running. Type 'help' for commands, 'stop' to exit.");

		ConsoleSender consoleSender;

		while (m_Running) {
			std::string input;
			if (!std::getline(std::cin, input)) {
				break;
			}

			if (input.empty()) {
				continue;
			}

			if (input == "stop") {
				m_Running = false;
				break;
			}

			m_CommandRegistry->Dispatch(consoleSender, input);
		}
	}

	void Application::Shutdown() {
		AX_CORE_INFO("Server shutting down...");

		m_NetworkServer->Stop();

		ServerStopEvent stopEvent;
		m_EventBus->Publish(stopEvent);

		m_PluginManager->DisableAll();

		m_Config->Save();

		AX_CORE_INFO("Server stopped.");
		s_Instance = nullptr;
	}

}
