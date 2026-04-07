#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Event/EventBus.h"
#include "Axiom/Plugin/PluginManager.h"
#include "Axiom/Plugin/PluginContext.h"
#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Network/NetworkServer.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/PacketRegistry.h"

namespace Axiom {

	class Application {
	public:
		Application() = default;
		~Application() = default;

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Init();
		void Run();
		void Shutdown()const;
		void Stop() { m_Running = false; }

		EventBus& Events() const { return *m_EventBus; }
		PluginManager& Plugins() const { return *m_PluginManager; }
		CommandRegistry& Commands() const { return *m_CommandRegistry; }
		ServerConfig& Config() const { return *m_Config; }

		static Application& Instance() { return *s_Instance; }

	private:
		void RegisterPackets();

		Scope<EventBus> m_EventBus;
		Scope<PluginManager> m_PluginManager;
		Scope<CommandRegistry> m_CommandRegistry;
		Scope<ServerConfig> m_Config;
		Scope<PluginContext> m_PluginContext;
		Scope<NetworkServer> m_NetworkServer;
		Scope<PacketContext> m_PacketContext;
		PacketRegistry m_PacketRegistry;
		bool m_Running = false;

		static Application* s_Instance;
	};

}
