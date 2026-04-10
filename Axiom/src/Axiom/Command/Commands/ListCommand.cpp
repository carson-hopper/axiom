#include "axpch.h"
#include "Axiom/Command/Commands/ListCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Environment/Entity/Player.h"

namespace Axiom {

    Ref<LiteralNode> ListCommand::BuildTree() {
        auto root = Literal("list");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            auto& server = Application::Instance().PacketCtx().Server();
            const int count = server.PlayerCount();
            std::string names;
            for (const auto& player : server.AllPlayers()) {
                if (!names.empty()) {
                    names += ", ";
                }
                names += player->Name();
            }
            std::string text = "There are " + std::to_string(count)
                + (count == 1 ? " player" : " players") + " online";
            if (!names.empty()) {
                text += ": " + names;
            }
            source.SendMessage(ChatComponent::Create()
                .Text(text)
                .Color(ChatColor::White)
                .Build());
            return 1;
        });

        return root;
    }

}
