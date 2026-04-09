#include "axpch.h"
#include "Axiom/Command/Commands/Server/RestartCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Chat/ChatComponent.h"

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
    #include <cerrno>
    #include <cstring>
    #include <unistd.h>
    extern char** environ;
#endif

namespace Axiom {

	void RestartCommand::Execute(CommandSender& sender, const std::vector<std::string>&) {
		sender.SendMessage(ChatComponent::Create()
			.Text("Saving chunks and restarting...")
			.Color(ChatColor::Yellow)
			.Build());

		AX_CORE_INFO("Server restart requested by {}", sender.Name());

		// Save all dirty chunks
		auto& app = Application::Instance();
		app.PacketCtx().ChunkManagement().SaveAllDirtyChunks();

		// Disconnect all players
		for (const auto& player : app.PacketCtx().Server().AllPlayers()) {
			if (player->GetConnection()->IsConnected()) {
				player->GetConnection()->Disconnect("Server restarting");
			}
		}

		AX_CORE_INFO("Server restarting...");

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
		auto exePath = ExecutableDirectory().string() + "/Axiom";

		// Stop the server and destroy it to release the port before exec
		app.Stop();
		app.~Application();

		char* argv[] = {const_cast<char*>(exePath.c_str()), nullptr};
		execve(argv[0], argv, ::environ);
		AX_CORE_ERROR("Failed to restart: {}", strerror(errno));
#else
		sender.SendPlainMessage("Restart not supported on this platform");
#endif
	}

	std::vector<std::string> RestartCommand::TabComplete(CommandSender&, const std::vector<std::string>&) {
		return {};
	}

}
