#include "axpch.h"
#include "GameruleCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Config/GameRules.h"
#include "Axiom/Core/Application.h"

namespace Axiom {

    Ref<LiteralNode> GameruleCommand::BuildTree() {
        auto root = Literal("gamerule");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            const auto& rules = Application::Instance().Rules();
            const auto names = rules.RuleNames();
            source.SendMessage(ChatComponent::Create()
                .Text("--- Game Rules ---")
                .Color(ChatColor::Gold)
                .Build());
            for (const auto& name : names) {
                auto value = rules.Get(name);
                source.SendMessage(ChatComponent::Create()
                    .Text(name + " = " + value.ToString())
                    .Color(ChatColor::Gray)
                    .Build());
            }
            return 1;
        });

        const auto rule = Argument("rule", CreateRef<StringParser>(StringParser::Mode::SingleWord));
        rule->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            const auto iterator = arguments.find("rule");
            if (iterator == arguments.end()) {
                return 0;
            }
            const std::string& ruleName = iterator->second;
            const auto& rules = Application::Instance().Rules();
            if (!rules.Has(ruleName)) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Unknown game rule: " + ruleName)
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            const auto value = rules.Get(ruleName);
            source.SendMessage(ChatComponent::Create()
                .Text(ruleName + " = " + value.ToString())
                .Color(ChatColor::White)
                .Build());
            return 1;
        });

        const auto value = Argument("value", CreateRef<StringParser>(StringParser::Mode::GreedyPhrase));
        value->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            const auto ruleIterator = arguments.find("rule");
            const auto valueIterator = arguments.find("value");
            if (ruleIterator == arguments.end() || valueIterator == arguments.end()) {
                return 0;
            }
            const std::string& ruleName = ruleIterator->second;
            const std::string& ruleValue = valueIterator->second;
            auto& rules = Application::Instance().Rules();
            if (!rules.Has(ruleName)) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Unknown game rule: " + ruleName)
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            if (!rules.SetFromString(ruleName, ruleValue)) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Failed to set " + ruleName + " to " + ruleValue)
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            source.SendMessage(ChatComponent::Create()
                .Text("Set " + ruleName + " to " + ruleValue)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        });
        rule->Then(value);

        root->Then(rule);

        return root;
    }

}
