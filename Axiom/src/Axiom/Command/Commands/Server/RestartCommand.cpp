#include "axpch.h"
#include "Axiom/Command/Commands/Server/RestartCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Core/PathUtil.h"
#include "Axiom/Environment/Entity/Player.h"

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
    #include <cerrno>
    #include <cstring>
    #include <unistd.h>
    extern char** environ;
#endif

namespace Axiom {

    Ref<LiteralNode> RestartCommand::BuildTree() {
        auto root = Literal("restart");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            source.SendMessage(ChatComponent::Create()
                .Text("Saving chunks and restarting...")
                .Color(ChatColor::Yellow)
                .Build());

            AX_CORE_INFO("Server restart requested by {}", source.GetName());

            auto& app = Application::Instance();
            app.PacketCtx().ChunkManagement().SaveAllDirtyChunks();

            for (const auto& player : app.PacketCtx().Server().AllPlayers()) {
                if (player->GetConnection()->IsConnected()) {
                    player->GetConnection()->Disconnect("Server restarting");
                }
            }

            AX_CORE_INFO("Server restarting...");

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
            auto exePath = ExecutableDirectory().string() + "/Axiom";

            app.Stop();
            app.~Application();

            char* argv[] = {const_cast<char*>(exePath.c_str()), nullptr};
            execve(argv[0], argv, ::environ);
            AX_CORE_ERROR("Failed to restart: {}", strerror(errno));
#else
            source.SendFailure(ChatComponent::Create()
                .Text("Restart not supported on this platform")
                .Color(ChatColor::Red)
                .Build());
#endif
            return 1;
        });

        return root;
    }

}
