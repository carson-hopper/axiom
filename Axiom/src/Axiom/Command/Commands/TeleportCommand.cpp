#include "axpch.h"
#include "Axiom/Command/Commands/TeleportCommand.h"

#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

    void TeleportCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to teleport")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        if (arguments.empty()) {
            sender.SendMessage(ChatComponent::Create()
                .Text("Usage: /teleport <target> [x y z]")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        const std::string& target = arguments[0];

        if (arguments.size() >= 4) {
            sender.SendMessage(ChatComponent::Create()
                .Text("Teleported " + target +
                    " to " + arguments[1] +
                    " " + arguments[2] +
                    " " + arguments[3])
                .Color(ChatColor::Green)
                .Build());
            return;
        }

        sender.SendMessage(ChatComponent::Create()
            .Text("Teleported to " + target)
            .Color(ChatColor::Green)
            .Build());
    }

    std::vector<std::string> TeleportCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {
        return {};
    }

}
