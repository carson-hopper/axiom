#include "axpch.h"
#include "Axiom/Core/Application.h"

#include "Axiom/Core/Assert.h"
#include "Axiom/Event/ServerEvents.h"
#include "Axiom/Plugin/CorePlugin.h"
#include "Axiom/Command/CommandSourceStack.h"

#include "Axiom/Network/Packet/PacketFactory.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace Axiom {

	Application* Application::s_Instance = nullptr;

	Application& Application::Instance() {
		if (s_Instance == nullptr) {
			AX_CORE_ERROR("Application::Instance() called with no active instance; aborting");
			std::exit(EXIT_FAILURE);
		}
		return *s_Instance;
	}

	Application::Application(ApplicationSpecification& specification)
		: m_Specification(std::move(specification)) {

		AX_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty()) {
			std::filesystem::current_path(m_Specification.WorkingDirectory);
		}
	}

	Application::~Application() {
		AX_CORE_TRACE("Shutting down Axiom Server");

		if (m_PacketContext) {
			m_PacketContext->ChunkManagement().SaveAllDirtyChunks();
		}

		m_ConsoleInput.Stop();
		m_Watchdog.Stop();
		m_TickScheduler.Stop();
		m_AsyncExecutor.Shutdown();

		s_Instance = nullptr;
	}

	void Application::PushTick(Ref<Tickable> tick) {
		m_TickRegistry.RegisterTickable(std::move(tick));
	}

	void Application::PopTick(Tickable* tick) {
		m_TickRegistry.UnregisterTickable(tick);
	}

	void Application::PushAsyncTick(Ref<AsyncTickable> asyncTick) {
		m_TickRegistry.RegisterAsyncTickable(std::move(asyncTick));
	}

	static bool CheckEula() {
		constexpr std::string eulaPath = "eula.txt";
		std::ifstream file(eulaPath);
		if (!file.good()) {
			std::ofstream create(eulaPath);
			create << "#By changing the setting below to TRUE you are indicating your agreement "
				   << "to the Minecraft EULA (https://aka.ms/MinecraftEULA).\n"
				   << "eula=false\n";
			AX_CORE_ERROR("You must agree to the EULA to run the server.");
			AX_CORE_ERROR("Edit eula.txt and set eula=true to accept.");
			return false;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#')
				continue;
			if (line.find("eula=true") != std::string::npos)
				return true;
			if (line.find("eula=yes") != std::string::npos)
				return true;
		}

		AX_CORE_ERROR("You must agree to the EULA to run the server.");
		AX_CORE_ERROR("Edit eula.txt and set eula=true to accept.");
		return false;
	}

	void Application::Init() {
		AX_CORE_INFO("Axiom Server v0.1.0 starting...");

		if (!CheckEula()) {
			m_Running = false;
			return;
		}

		m_Config = CreateScope<ServerConfig>();
		m_Config->Load("server.toml");

		m_AdminFiles.LoadAll();

		m_EventBus = CreateScope<EventBus>();
		m_CommandRegistry = CreateScope<CommandRegistry>();
		m_PluginManager = CreateScope<PluginManager>();

		m_PluginContext = CreateScope<PluginContext>(*m_EventBus, *m_CommandRegistry, *m_Config);
		m_PluginManager->RegisterPlugin(CreateScope<CorePlugin>());
		m_PluginManager->EnableAll(*m_PluginContext);

		m_NetworkServer = Ref<NetworkServer>::Create();
		m_PacketContext = CreateScope<PacketContext>(*m_Config, *m_EventBus, *m_CommandRegistry, *m_NetworkServer, m_AdminFiles);
		PacketFactory::RegisterAll();

		m_NetworkServer->SetPacketHandler([this](const Ref<Connection>& connection, const int32_t packetId, NetworkBuffer& buffer) {
			const auto state = static_cast<PacketState>(connection->State());
			PacketFactory::DispatchPacket(state, packetId, connection, *m_PacketContext, buffer);
		});
		m_NetworkServer->Start(m_Config->Port());

		AX_CORE_INFO("Server initialized on port {}", m_Config->Port());
	}

	void Application::Run() {
		m_Running = true;

		ServerStartEvent startEvent;
		m_EventBus->Publish(startEvent);

		AX_CORE_INFO("Server is running. Type 'help' for commands, 'stop' to exit.");

		m_Watchdog.Start();

		m_ConsoleInput.ConfigureDefaultPrompt({
			.PlayerCount = [this] { return m_PacketContext->Server().PlayerCount(); },
			.Tps = [this] { return m_TickScheduler.ActualTPS(); },
		});
		m_ConsoleInput.StartDispatchingCommands(*m_CommandRegistry);

		m_TickScheduler.RunSyncLoop([this]() -> bool {
			m_Watchdog.TickStarted();

			const bool shouldContinue = m_Running;

			m_Watchdog.TickFinished();
			return shouldContinue;
		}, 20.0f);
	}

}
