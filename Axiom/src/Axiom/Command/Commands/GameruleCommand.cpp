#include "axpch.h"
#include "Axiom/Command/Commands/GameruleCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Config/GameRules.h"

namespace Axiom {

    void GameruleCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to manage game rules")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        auto& rules = Application::Instance().Rules();

        // No arguments: list all rules
        if (arguments.empty()) {
            auto names = rules.RuleNames();
            sender.SendMessage(ChatComponent::Create()
                .Text("--- Game Rules ---")
                .Color(ChatColor::Gold)
                .Build());
            for (const auto& name : names) {
                auto value = rules.Get(name);
                sender.SendMessage(ChatComponent::Create()
                    .Text(name + " = " + value.ToString())
                    .Color(ChatColor::Gray)
                    .Build());
            }
            return;
        }

        const std::string& ruleName = arguments[0];

        // One argument: query a specific rule
        if (arguments.size() == 1) {
            if (!rules.Has(ruleName)) {
                sender.SendMessage(ChatComponent::Create()
                    .Text("Unknown game rule: " + ruleName)
                    .Color(ChatColor::Red)
                    .Build());
                return;
            }
            auto value = rules.Get(ruleName);
            sender.SendMessage(ChatComponent::Create()
                .Text(ruleName + " = " + value.ToString())
                .Color(ChatColor::White)
                .Build());
            return;
        }

        // Two arguments: set a rule
        const std::string& ruleValue = arguments[1];
        if (!rules.SetFromString(ruleName, ruleValue)) {
            sender.SendMessage(ChatComponent::Create()
                .Text("Failed to set " + ruleName + " to " + ruleValue)
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        sender.SendMessage(ChatComponent::Create()
            .Text("Set " + ruleName + " to " + ruleValue)
            .Color(ChatColor::Green)
            .Build());
    }

    std::vector<std::string> GameruleCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
        if (arguments.size() <= 1) {
            auto& rules = Application::Instance().Rules();
            auto names = rules.RuleNames();
            if (arguments.empty()) {
                return names;
            }
            std::vector<std::string> results;
            for (const auto& name : names) {
                if (name.find(arguments[0]) == 0) {
                    results.push_back(name);
                }
            }
            return results;
        }
        return {};
    }

}
