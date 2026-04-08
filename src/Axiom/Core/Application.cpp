#include "axpch.h"
#include "Axiom/Core/Application.h"

#include "Axiom/Core/Time.h"
#include "Axiom/Core/Assert.h"
#include "Axiom/Event/ServerEvents.h"
#include "Axiom/Plugin/CorePlugin.h"
#include "Axiom/Command/CommandSender.h"

#include "Axiom/Network/Packet/26.1/Packets.h"

#include "Axiom/Environment/Level/LevelTime.h"

#include <iostream>
#include <string>
#include <charconv>

namespace Axiom {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification) {

		AX_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);
	}

	Application::~Application()
	{
		AX_CORE_TRACE("Shutting down Axiom Server");
		
		// Stop the tick stack first
		m_TickStack.Stop();
		
		s_Instance = nullptr;
	}

	void Application::PushTick(Ref<Tickable> tick)
	{
		m_TickStack.PushTick(std::move(tick));
	}

	void Application::PopTick(Tickable* tick)
	{
		m_TickStack.PopTick(tick);
	}

	void Application::Init() {
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
			[this](const Ref<Connection>& connection, const int32_t packetId, NetworkBuffer& buffer) {
				m_PacketRegistry.Dispatch(connection->State(), packetId, buffer,
					connection, *m_PacketContext);
			});
		m_NetworkServer->Start(m_Config->Port());

		// Register server commands that need PacketContext
		m_CommandRegistry->Register("time", "Set time of day (0-24000 or day/noon/night/midnight)",
	[this](CommandSender& sender, const std::vector<std::string>& arguments) {
			if (arguments.empty()) {
				sender.SendPlainMessage("Time: " + std::to_string(m_PacketContext->Time().TimeOfDay()));
				return;
			}

			const std::string_view argument = arguments[0];

			constexpr std::pair<std::string_view, int64_t> timeMap[] = {
				{"day", 1000},
				{"noon", 6000},
				{"sunset", 12000},
				{"night", 13000},
				{"midnight", 18000}
			};

			for (const auto& [name, value] : timeMap) {
				if (argument == name) {
					m_PacketContext->Time().SetTimeOfDay(value);
					sender.SendPlainMessage("Set time to " + std::to_string(value));
					return;
				}
			}

			int64_t parsed;
			auto [ptr, ec] = std::from_chars(argument.data(), argument.data() + argument.size(), parsed);

			if (ec != std::errc() || ptr != argument.data() + argument.size()) {
				sender.SendPlainMessage("Invalid time: " + std::string(argument));
				return;
			}

			if (parsed < 0 || parsed > 24000) {
				sender.SendPlainMessage("Time must be between 0 and 24000");
				return;
			}

			m_PacketContext->Time().SetTimeOfDay(parsed);
			sender.SendPlainMessage("Set time to " + std::to_string(parsed));
		});

		m_CommandRegistry->Register("weather", "Set weather (clear/rain/thunder)",
	[this](CommandSender& sender, const std::vector<std::string>& arguments) {
			if (arguments.empty()) {
				sender.SendPlainMessage("Usage: weather <clear|rain|thunder>");
				return;
			}

			const std::string_view argument = arguments[0];

			constexpr std::pair<std::string_view, WeatherType> weatherMap[] = {
				{"clear", WeatherType::Clear},
				{"rain", WeatherType::Rain},
				{"thunder", WeatherType::Thunder}
			};

			for (const auto& [name, type] : weatherMap) {
				if (argument == name) {
					m_PacketContext->Time().SetWeather(type);
					sender.SendPlainMessage("Weather set to " + std::string(name));
					return;
				}
			}

			sender.SendPlainMessage("Unknown weather: " + std::string(argument));
		});

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

		// Configuration
		m_PacketRegistry.Register<775, ClientInformationPacket<775>>();
		m_PacketRegistry.Register<775, SelectKnownPacksPacket<775>>();
		m_PacketRegistry.Register<775, FinishConfigurationPacket<775>>();
		m_PacketRegistry.Register<775, ConfigKeepAlivePacket<775>>();

		// Play
		m_PacketRegistry.Register<775, PlayKeepAlivePacket<775>>();
		m_PacketRegistry.Register<775, PlayerLoadedPacket<775>>();
		m_PacketRegistry.Register<775, ChunkBatchReceivedPacket<775>>();
		m_PacketRegistry.Register<775, AcceptTeleportationPacket<775>>();
		m_PacketRegistry.Register<775, ClientTickEndPacket<775>>();
		m_PacketRegistry.Register<775, MovePlayerPositionPacket<775>>();
		m_PacketRegistry.Register<775, MovePlayerPositionRotationPacket<775>>();
		m_PacketRegistry.Register<775, MovePlayerRotationPacket<775>>();
		m_PacketRegistry.Register<775, MovePlayerStatusOnlyPacket<775>>();
		m_PacketRegistry.Register<775, PlayerActionPacket<775>>();
		m_PacketRegistry.Register<775, UseItemOnPacket<775>>();
		m_PacketRegistry.Register<775, SetCarriedItemPacket<775>>();
		m_PacketRegistry.Register<775, SetCreativeModeSlotPacket<775>>();
	}

	void Application::Run() {
		m_Running = true;

		ServerStartEvent startEvent;
		m_EventBus->Publish(startEvent);

		AX_CORE_INFO("Server is running. Type 'help' for commands, 'stop' to exit.");

		ConsoleSender consoleSender;
		m_TickStack.RunSyncLoop([this, &consoleSender]() -> bool {
			// Process console input each tick
			if (std::cin.peek() != EOF) {
				if (std::string input ;std::getline(std::cin, input)) {
					if (!input.empty()) {
						if (input == "stop") {
							m_Running = false;
							return false;
						}
						m_CommandRegistry->Dispatch(consoleSender, input);
					}
				}
			}

			return m_Running;
		}, 20.0f);
	}

}
