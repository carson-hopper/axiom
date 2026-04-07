#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/TickStack.h"

#include "Axiom/Event/EventBus.h"

#include "Axiom/Command/CommandRegistry.h"

#include "Axiom/Config/ServerConfig.h"

#include "Axiom/Network/NetworkServer.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/PacketRegistry.h"

#include "Axiom/Plugin/PluginManager.h"
#include "Axiom/Plugin/PluginContext.h"

namespace Axiom {

	struct ApplicationCommandLineArgs {
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](const int index) const {
			return Args[index];
		}
	};

	struct ApplicationSpecification {
		std::string Name = "Axiom";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application {
	public:
		explicit Application(const ApplicationSpecification& specification);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void PushTick(Tickable* tick);
		void PopTick(Tickable* tick);

		TickStack& GetTickStack() { return m_LayerStack; }

		void Init();
		void Run();
		void Stop() { m_Running = false; }

		EventBus& Events() const { return *m_EventBus; }
		PluginManager& Plugins() const { return *m_PluginManager; }
		CommandRegistry& Commands() const { return *m_CommandRegistry; }
		ServerConfig& Config() const { return *m_Config; }

		static Application& Instance() { return *s_Instance; }

	private:
		void RegisterPackets();

	private:
		ApplicationSpecification m_Specification;
		Scope<EventBus> m_EventBus;
		Scope<PluginManager> m_PluginManager;
		Scope<CommandRegistry> m_CommandRegistry;
		Scope<ServerConfig> m_Config;
		Scope<PluginContext> m_PluginContext;
		Scope<NetworkServer> m_NetworkServer;
		Scope<PacketContext> m_PacketContext;
		PacketRegistry m_PacketRegistry;

		TickStack m_LayerStack;

		bool m_Running = false;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	Scope<Application> CreateApplication(ApplicationCommandLineArgs args);


}
