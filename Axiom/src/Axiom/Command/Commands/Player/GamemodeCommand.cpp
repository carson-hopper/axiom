#include "axpch.h"
#include "GamemodeCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Environment/Entity/Player.h"

namespace Axiom {

    namespace {
        int SetGameModeFor(CommandSourceStack& source, const GameMode mode, const std::string& label) {
            auto player = source.GetPlayer();
            if (!player) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Only players can change game mode")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            player->SetGameMode(mode);
            source.SendMessage(ChatComponent::Create()
                .Text("Set game mode to " + label)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        }
    }

    Ref<LiteralNode> GamemodeCommand::BuildTree() {
        auto root = Literal("gamemode");
        root->Requires(RequiredPermissionLevel());

        const auto survival = Literal("survival");
        survival->Executes([](CommandSourceStack& source, auto&) {
            return SetGameModeFor(source, GameMode::Survival, "survival");
        });
        root->Then(survival);

        const auto creative = Literal("creative");
        creative->Executes([](CommandSourceStack& source, auto&) {
            return SetGameModeFor(source, GameMode::Creative, "creative");
        });
        root->Then(creative);

        const auto adventure = Literal("adventure");
        adventure->Executes([](CommandSourceStack& source, auto&) {
            return SetGameModeFor(source, GameMode::Adventure, "adventure");
        });
        root->Then(adventure);

        const auto spectator = Literal("spectator");
        spectator->Executes([](CommandSourceStack& source, auto&) {
            return SetGameModeFor(source, GameMode::Spectator, "spectator");
        });
        root->Then(spectator);

    	for (const auto& children : root->GetChildren()) {
			children->Requires(RequiredPermissionLevel());
    		children->Requires(RequiredPermission() + "." + children->GetName());
    	}

        return root;
    }

}
