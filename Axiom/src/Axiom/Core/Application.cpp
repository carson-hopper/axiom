#include "axpch.h"
#include "Axiom/Core/Application.h"

#include "Axiom/Core/Time.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Core/BuildCount.generated.h"
#include "Axiom/Core/Assert.h"
#include "Axiom/Event/ServerEvents.h"
#include "Axiom/Plugin/CorePlugin.h"
#include "Axiom/Command/CommandSender.h"

#include "Axiom/Network/Packet/PacketFactory.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


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

		// Save all modified chunks before shutdown
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

		m_NetworkServer = CreateRef<NetworkServer>();
		m_PacketContext = CreateScope<PacketContext>(*m_Config, *m_EventBus, *m_CommandRegistry, *m_NetworkServer);
		PacketFactory::RegisterAll();

		m_NetworkServer->SetPacketHandler(
			[this](Ref<Connection> connection, const int32_t packetId, NetworkBuffer& buffer) {
				auto state = static_cast<PacketState>(connection->State());
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

		ConsoleSender consoleSender;
		// Starship-style prompt segments
		m_ConsoleInput.Prompt().AddLeft([]() -> ConsolePrompt::Segment {
			return {"", "axiom", 15, 62};
		});
		m_ConsoleInput.Prompt().AddLeft([]() -> ConsolePrompt::Segment {
#ifdef AX_DEBUG
			return {"", "v26.1-" AX_STRINGIFY_MACRO(AX_COMMIT_COUNT) "+b" AX_STRINGIFY_MACRO(AX_BUILD_COUNT), 15, 33};
#else
			return {"", "v26.1-" AX_STRINGIFY_MACRO(AX_COMMIT_COUNT), 15, 33};
#endif
		});
		m_ConsoleInput.Prompt().AddRight([this]() -> ConsolePrompt::Segment {
			int count = static_cast<int>(m_PacketContext->Server().PlayerCount());
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
