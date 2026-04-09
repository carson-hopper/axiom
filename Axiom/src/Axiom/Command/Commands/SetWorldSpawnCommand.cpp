#include "axpch.h"
#include "Axiom/Command/Commands/SetWorldSpawnCommand.h"

#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

    void SetWorldSpawnCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to set world spawn")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        if (arguments.size() >= 3) {
            sender.SendMessage(ChatComponent::Create()
                .Text("Set world spawn to " +
                    arguments[0] + " " +
                    arguments[1] + " " +
                    arguments[2])
                .Color(ChatColor::Green)
                .Build());
            return;
        }

        sender.SendMessage(ChatComponent::Create()
            .Text("Set world spawn to current position")
            .Color(ChatColor::Green)
            .Build());
    }

    std::vector<std::string> SetWorldSpawnCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {
        return {};
    }

}
