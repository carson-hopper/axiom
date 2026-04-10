#include "axpch.h"
#include "Axiom/Command/Commands/Environment/DifficultyCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Core/Application.h"

namespace Axiom {

    namespace {
        int SendSetMessage(CommandSourceStack& source, const std::string& level) {
            source.SendMessage(ChatComponent::Create()
                .Text("Set difficulty to " + level)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        }
    }

    Ref<LiteralNode> DifficultyCommand::BuildTree() {
        auto root = Literal("difficulty");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            const auto& config = Application::Instance().Config();
            source.SendMessage(ChatComponent::Create()
                .Text("Current difficulty: " + config.Difficulty())
                .Color(ChatColor::White)
                .Build());
            return 1;
        });

        const auto peaceful = Literal("peaceful");
        peaceful->Executes([](CommandSourceStack& source, auto&) {
            return SendSetMessage(source, "peaceful");
        });
        root->Then(peaceful);

        const auto easy = Literal("easy");
        easy->Executes([](CommandSourceStack& source, auto&) {
            return SendSetMessage(source, "easy");
        });
        root->Then(easy);

        const auto normal = Literal("normal");
        normal->Executes([](CommandSourceStack& source, auto&) {
            return SendSetMessage(source, "normal");
        });
        root->Then(normal);

        const auto hard = Literal("hard");
        hard->Executes([](CommandSourceStack& source, auto&) {
            return SendSetMessage(source, "hard");
        });
        root->Then(hard);

        return root;
    }

}
