#include "axpch.h"
#include "Axiom/Command/Commands/DifficultyCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

    void DifficultyCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to change difficulty")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        if (arguments.empty()) {
            auto& config = Application::Instance().Config();
            sender.SendMessage(ChatComponent::Create()
                .Text("Current difficulty: " + config.Difficulty())
                .Color(ChatColor::White)
                .Build());
            return;
        }

        const std::string& level = arguments[0];
        sender.SendMessage(ChatComponent::Create()
            .Text("Set difficulty to " + level)
            .Color(ChatColor::Green)
            .Build());
    }

    std::vector<std::string> DifficultyCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
        if (arguments.size() <= 1) {
            std::vector<std::string> levels = {"peaceful", "easy", "normal", "hard"};
            if (arguments.empty()) {
                return levels;
            }
            std::vector<std::string> results;
            for (const auto& level : levels) {
                if (level.find(arguments[0]) == 0) {
                    results.push_back(level);
                }
            }
            return results;
        }
        return {};
    }

}
