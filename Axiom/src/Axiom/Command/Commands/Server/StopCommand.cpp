#include "axpch.h"
#include "Axiom/Command/Commands/Server/StopCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"

namespace Axiom {

    Ref<LiteralNode> StopCommand::BuildTree() {
        auto root = Literal("stop");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            source.SendMessage(ChatComponent::Create()
                .Text("Stopping the server...")
                .Color(ChatColor::Yellow)
                .Build());

            AX_CORE_INFO("Server stop requested by {}", source.GetName());

        	for (const auto& player : Application::Instance().Server()->AllPlayers()) {
        		player->Kick("Server Stopped");
        	}

            Application::Instance().Stop();
            return 1;
        });

        return root;
    }

}
