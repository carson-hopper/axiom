#include "axpch.h"
#include "Axiom/Core/Application.h"

#include "Axiom/Core/Time.h"
#include "Axiom/Core/Assert.h"
#include "Axiom/Event/ServerEvents.h"
#include "Axiom/Plugin/CorePlugin.h"
#include "Axiom/Command/CommandSender.h"

#include "Axiom/Network/Packet/PacketFactory.h"

#include "Axiom/Environment/Level/LevelTime.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <charconv>

#if defined(AX_PLATFORM_MACOS)
#include <mach/mach.h>
#endif

namespace Axiom {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification) {

		AX_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);
	}

	Application::~Application() {
		AX_CORE_TRACE("Shutting down Axiom Server");

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
		const std::string eulaPath = "eula.txt";
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

		m_PacketContext = CreateScope<PacketContext>(*m_Config, *m_EventBus, *m_CommandRegistry);
		PacketFactory::RegisterAll();

		m_NetworkServer = CreateScope<NetworkServer>();
		m_NetworkServer->SetPacketHandler(
			[this](Ref<Connection> connection, const int32_t packetId, NetworkBuffer& buffer) {
				auto state = static_cast<PacketState>(connection->State());
				PacketFactory::DispatchPacket(state, packetId, connection, *m_PacketContext, buffer);
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

	void Application::Run() {
		m_Running = true;

		ServerStartEvent startEvent;
		m_EventBus->Publish(startEvent);

		AX_CORE_INFO("Server is running. Type 'help' for commands, 'stop' to exit.");

		m_Watchdog.Start();

		ConsoleSender consoleSender;
		// Starship-style prompt segments
		m_ConsoleInput.Prompt().AddLeft([]() -> ConsolePrompt::Segment {
			return {"", "axiom", 15, 62};
		});
		m_ConsoleInput.Prompt().AddLeft([]() -> ConsolePrompt::Segment {
			return {"", "v26.1", 15, 33};
		});
		m_ConsoleInput.Prompt().AddRight([this]() -> ConsolePrompt::Segment {
			int count = static_cast<int>(m_PacketContext->Players().PlayerCount());
			std::string text = std::to_string(count) + (count == 1 ? " player" : " players");
			return {"", text, 15, count > 0 ? 62 : 240};
		});
		m_ConsoleInput.Prompt().AddRight([this]() -> ConsolePrompt::Segment {
			float tps = m_TickScheduler.ActualTPS();
			char tpsText[16];
			std::snprintf(tpsText, sizeof(tpsText), "%.1f TPS", tps);
			int background = tps >= 19.0f ? 28 : (tps >= 15.0f ? 136 : 124);
			return {"", tpsText, 15, background};
		});
		m_ConsoleInput.Prompt().AddRight([]() -> ConsolePrompt::Segment {
			// Memory usage (RSS)
			size_t memoryBytes = 0;
#if defined(AX_PLATFORM_MACOS)
			struct mach_task_basic_info info{};
			mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
			if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
				reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS) {
				memoryBytes = info.resident_size;
			}
#elif defined(AX_PLATFORM_LINUX)
			std::ifstream statusFile("/proc/self/status");
			std::string line;
			while (std::getline(statusFile, line)) {
				if (line.starts_with("VmRSS:")) {
					memoryBytes = std::stoull(line.substr(6)) * 1024;
					break;
				}
			}
#endif
			char memText[32];
			if (memoryBytes >= 1024ULL * 1024 * 1024) {
				std::snprintf(memText, sizeof(memText), "%.1f GB",
					static_cast<double>(memoryBytes) / (1024.0 * 1024.0 * 1024.0));
			} else {
				std::snprintf(memText, sizeof(memText), "%.0f MB",
					static_cast<double>(memoryBytes) / (1024.0 * 1024.0));
			}
			return {"", memText, 15, 97};
		});
		m_ConsoleInput.Prompt().AddRight([]() -> ConsolePrompt::Segment {
			// CPU usage (process)
			double cpuPercent = 0.0;
#if defined(AX_PLATFORM_MACOS)
			thread_array_t threadList;
			mach_msg_type_number_t threadCount;
			if (task_threads(mach_task_self(), &threadList, &threadCount) == KERN_SUCCESS) {
				for (mach_msg_type_number_t i = 0; i < threadCount; i++) {
					thread_basic_info_data_t threadInfo;
					mach_msg_type_number_t infoCount = THREAD_BASIC_INFO_COUNT;
					if (thread_info(threadList[i], THREAD_BASIC_INFO,
						reinterpret_cast<thread_info_t>(&threadInfo), &infoCount) == KERN_SUCCESS) {
						if (!(threadInfo.flags & TH_FLAGS_IDLE)) {
							cpuPercent += static_cast<double>(threadInfo.cpu_usage) / TH_USAGE_SCALE * 100.0;
						}
					}
					mach_port_deallocate(mach_task_self(), threadList[i]);
				}
				vm_deallocate(mach_task_self(),
					reinterpret_cast<vm_address_t>(threadList),
					threadCount * sizeof(thread_t));
			}
#endif
			// Normalize to 0-100% across all cores
			unsigned int coreCount = std::thread::hardware_concurrency();
			if (coreCount > 0) cpuPercent /= coreCount;

			char cpuText[16];
			std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuPercent);
			int background = cpuPercent < 50.0 ? 24 : (cpuPercent < 80.0 ? 136 : 124);
			return {"", cpuText, 15, background};
		});
		m_ConsoleInput.Prompt().SetPromptChar("\xe2\x9d\xaf"); // ❯
		m_ConsoleInput.Prompt().SetPromptColor(76); // green

		m_ConsoleInput.SetCompletionProvider([this](const std::string& partial) -> std::vector<std::string> {
			std::vector<std::string> results;
			for (const auto& name : m_CommandRegistry->GetCommandNames()) {
				if (name.starts_with(partial)) {
					results.push_back(name);
				}
			}
			if (results.empty()) {
				results.push_back("stop");
			}
			return results;
		});
		m_ConsoleInput.Start([this, &consoleSender](const std::string& input) {
			if (input == "stop") {
				m_Running = false;
				m_TickScheduler.Stop();
				return;
			}
			m_CommandRegistry->Dispatch(consoleSender, input);
		});

		m_TickScheduler.RunSyncLoop([this]() -> bool {
			m_Watchdog.TickStarted();

			// Return value controls the loop
			bool shouldContinue = m_Running;

			m_Watchdog.TickFinished();
			return shouldContinue;
		}, 20.0f);
	}

}
