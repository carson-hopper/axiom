#include "axpch.h"
#include "Axiom/Command/Commands/GamemodeCommand.h"

#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

    void GamemodeCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to change game mode")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        if (arguments.empty()) {
            sender.SendMessage(ChatComponent::Create()
                .Text("Usage: /gamemode <survival|creative|adventure|spectator>")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        const std::string& mode = arguments[0];
        sender.SendMessage(ChatComponent::Create()
            .Text("Set game mode to " + mode)
            .Color(ChatColor::Green)
            .Build());
    }

    std::vector<std::string> GamemodeCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
        if (arguments.size() <= 1) {
            std::vector<std::string> modes = {"survival", "creative", "adventure", "spectator"};
            if (arguments.empty()) {
                return modes;
            }
            std::vector<std::string> results;
            for (const auto& mode : modes) {
                if (mode.find(arguments[0]) == 0) {
                    results.push_back(mode);
                }
            }
            return results;
        }
        return {};
    }

}
