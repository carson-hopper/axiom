#include "axpch.h"
#include "Axiom/Command/Commands/Player/TeleportCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Environment/Entity/Player.h"

#include <stdexcept>
#include <string>

namespace Axiom {

    Ref<LiteralNode> TeleportCommand::BuildTree() {
        auto root = Literal("tp");
        root->Requires(RequiredPermissionLevel());

        const auto target = Argument("target", Ref<EntityParser>::Create(true, false));
        target->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            const auto iterator = arguments.find("target");
            const std::string& targetName = iterator != arguments.end() ? iterator->second : "";
            source.SendMessage(ChatComponent::Create()
                .Text("Teleported to " + targetName)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        });

        const auto xArgument = Argument("x", Ref<DoubleParser>::Create());
        const auto yArgument = Argument("y", Ref<DoubleParser>::Create());
        const auto zArgument = Argument("z", Ref<DoubleParser>::Create());

        zArgument->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            auto player = source.GetPlayer();
            if (!player) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Only players can teleport to coordinates")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }

            const auto xIterator = arguments.find("x");
            const auto yIterator = arguments.find("y");
            const auto zIterator = arguments.find("z");
            if (xIterator == arguments.end() || yIterator == arguments.end() || zIterator == arguments.end()) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Missing teleport coordinates")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }

            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            try {
                x = std::stod(xIterator->second);
                y = std::stod(yIterator->second);
                z = std::stod(zIterator->second);
            } catch (const std::exception&) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Invalid coordinates")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }

            player->SetPosition(Vector3{x, y, z});
            source.SendMessage(ChatComponent::Create()
                .Text("Teleported to "
                    + xIterator->second + " "
                    + yIterator->second + " "
                    + zIterator->second)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        });

        yArgument->Then(zArgument);
        xArgument->Then(yArgument);
        target->Then(xArgument);

        root->Then(target);

        return root;
    }

}
