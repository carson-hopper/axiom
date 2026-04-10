#include "axpch.h"
#include "SetWorldSpawnCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Environment/Entity/Player.h"

namespace Axiom {

    Ref<LiteralNode> SetWorldSpawnCommand::BuildTree() {
        auto root = Literal("setspawn");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            auto player = source.GetPlayer();
            if (!player) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Only players can use /setspawn without coordinates")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            const auto& position = player->GetPosition();
            source.SendMessage(ChatComponent::Create()
                .Text("Set world spawn to "
                    + std::to_string(static_cast<int>(position.x)) + " "
                    + std::to_string(static_cast<int>(position.y)) + " "
                    + std::to_string(static_cast<int>(position.z)))
                .Color(ChatColor::Green)
                .Build());
            return 1;
        });

        const auto positionArgument = Argument("pos", CreateRef<BlockPosParser>());
        positionArgument->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            const auto iterator = arguments.find("pos");
            const std::string& positionText = iterator != arguments.end() ? iterator->second : "";
            source.SendMessage(ChatComponent::Create()
                .Text("Set world spawn to " + positionText)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        });
        root->Then(positionArgument);

        return root;
    }

}
