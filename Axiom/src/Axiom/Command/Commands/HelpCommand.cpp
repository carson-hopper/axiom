#include "axpch.h"
#include "Axiom/Command/Commands/HelpCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandDispatcher.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"

#include <algorithm>

namespace Axiom {

    HelpCommand::HelpCommand(CommandRegistry& registry)
        : m_Registry(registry) {}

    Ref<LiteralNode> HelpCommand::BuildTree() {
        auto root = Literal("help");
        root->Requires(RequiredPermissionLevel());

        auto* registry = &m_Registry;

        root->Executes([registry](CommandSourceStack& source, auto&) {
            source.SendMessage(ChatComponent::Create()
                .Text("=== ")
                .Append("Available Commands", ChatColor::Gold)
                .Append(" ===")
                .Build());

            source.SendMessage(ChatComponent::Create()
                .Text("Use ")
                .Append("/help <command>", ChatColor::Yellow)
                .Append(" for detailed info")
                .Build());

            // Only list commands the source is allowed to run.
            std::vector<std::string> accessibleNames;
            for (const auto& rootNode : registry->GetRootNodes()) {
                if (CommandDispatcher::CheckPermission(source, *rootNode)) {
                    accessibleNames.push_back(rootNode->GetName());
                }
            }
            std::ranges::sort(accessibleNames);

            for (const auto& name : accessibleNames) {
                source.SendMessage(ChatComponent::Create()
                    .Text("/" + name)
                    .Color(ChatColor::Gray)
                    .Build());
            }
            return 1;
        });

        const auto commandArgument = Argument("command", CreateRef<StringParser>(StringParser::Mode::SingleWord));
        commandArgument->Executes([registry](CommandSourceStack& source,
            const std::unordered_map<std::string, std::string>& arguments) {

            const auto iterator = arguments.find("command");
            if (iterator == arguments.end()) {
                return 0;
            }
            const std::string& commandName = iterator->second;

            // Locate the matching root node and verify the source can
            // run it. Treat inaccessible commands as if they don't
            // exist so we don't leak their existence.
            Ref<LiteralNode> matchedRoot;
            for (const auto& rootNode : registry->GetRootNodes()) {
                if (rootNode->GetName() == commandName) {
                    matchedRoot = rootNode;
                    break;
                }
            }

            if (!matchedRoot
                || !CommandDispatcher::CheckPermission(source, *matchedRoot)) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Unknown command: ")
                    .Append(commandName, ChatColor::Red)
                    .Build());
                return 0;
            }

            source.SendMessage(ChatComponent::Create()
                .Text("=== Command: ")
                .Append(commandName, ChatColor::Gold)
                .Append(" ===")
                .Build());
            source.SendMessage(ChatComponent::Create()
                .Text("Usage: /" + commandName)
                .Color(ChatColor::Yellow)
                .Build());
            return 1;
        });
        root->Then(commandArgument);

        return root;
    }

}
