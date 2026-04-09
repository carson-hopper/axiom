#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/TaskQueue.h"
#include "Axiom/Core/TickRegistry.h"
#include "Axiom/Core/TickScheduler.h"
#include "Axiom/Core/AsyncExecutor.h"
#include "Axiom/Core/Watchdog.h"
#include "Axiom/Core/ConsoleInput.h"

#include "Axiom/Event/EventBus.h"

#include "Axiom/Command/CommandRegistry.h"

#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Config/GameRules.h"
#include "Axiom/Config/AdminFileStore.h"

#include "Axiom/Network/NetworkServer.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/PacketFactory.h"

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

		void PushTick(Ref<Tickable> tick);
		void PopTick(Tickable* tick);
		void PushAsyncTick(Ref<AsyncTickable> asyncTick);

		TaskQueue& GetTaskQueue() { return m_TaskQueue; }
		TickRegistry& GetTickRegistry() { return m_TickRegistry; }
		TickScheduler& GetTickScheduler() { return m_TickScheduler; }
		AsyncExecutor& GetAsyncExecutor() { return m_AsyncExecutor; }

		void Init();
		void Run();
		void Stop() { m_Running = false; }

		EventBus& Events() const { return *m_EventBus; }
		PluginManager& Plugins() const { return *m_PluginManager; }
		CommandRegistry& Commands() const { return *m_CommandRegistry; }
		ServerConfig& Config() const { return *m_Config; }
		GameRules& Rules() { return m_GameRules; }
        AdminFileStore& AdminFiles() { return m_AdminFiles; }
        PacketContext& PacketCtx() const { return *m_PacketContext; }
        
        Ref<NetworkServer> Server() const { return m_NetworkServer; }

		static Application& Instance() { return *s_Instance; }

	private:
		ApplicationSpecification m_Specification;
		Scope<EventBus> m_EventBus;
		Scope<PluginManager> m_PluginManager;
		Scope<CommandRegistry> m_CommandRegistry;
		Scope<ServerConfig> m_Config;
		Scope<PluginContext> m_PluginContext;
		Ref<NetworkServer> m_NetworkServer;
		Scope<PacketContext> m_PacketContext;

		TaskQueue m_TaskQueue;
		TickRegistry m_TickRegistry;
		AsyncExecutor m_AsyncExecutor;
		TickScheduler m_TickScheduler{m_TickRegistry, m_TaskQueue, m_AsyncExecutor};
		Watchdog m_Watchdog;
		ConsoleInput m_ConsoleInput{m_TaskQueue};

		GameRules m_GameRules;
		AdminFileStore m_AdminFiles;

		bool m_Running = false;
	private:
		static Application* s_Instance;
	};

	Scope<Application> CreateApplication(ApplicationCommandLineArgs args);

}
