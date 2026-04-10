#include "Axiom/Command/CommandDispatcher.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Log.h"

#include <algorithm>
#include <sstream>

namespace Axiom {

CommandNode& CommandDispatcher::Register(Ref<LiteralNode> literal) {
	m_RootNodes.push_back(literal);
	return *literal;
}

int CommandDispatcher::Dispatch(CommandSourceStack& source, const std::string& input) {
	auto tokens = Tokenize(input);
	if (tokens.empty()) {
		return 0;
	}

	for (auto& root : m_RootNodes) {
		if (root->Matches(tokens[0])) {
			if (!source.HasPermission(root->GetRequiredPermissionLevel())) {
				source.SendFailure(ChatComponent::Create()
					.Text("You do not have permission to run this command")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			std::unordered_map<std::string, std::string> arguments;
			int result = Execute(source, root, tokens, 1, arguments);
			if (result != -1) {
				return result;
			}

			source.SendFailure(ChatComponent::Create()
				.Text("Invalid command syntax")
				.Color(ChatColor::Red)
				.Build());
			return 0;
		}
	}

	source.SendFailure(ChatComponent::Create()
		.Text("Unknown command: " + tokens[0])
		.Color(ChatColor::Red)
		.Build());
	return 0;
}

std::vector<std::string> CommandDispatcher::GetSuggestions(
	CommandSourceStack& source, const std::string& partial) {
	auto tokens = Tokenize(partial);
	std::vector<std::string> suggestions;

	bool endsWithSpace = !partial.empty() && partial.back() == ' ';

	if (tokens.empty() || (tokens.size() == 1 && !endsWithSpace)) {
		std::string prefix = tokens.empty() ? "" : tokens[0];
		for (auto& root : m_RootNodes) {
			if (!source.HasPermission(root->GetRequiredPermissionLevel())) {
				continue;
			}
			auto nodeSuggestions = root->ListSuggestions(source, prefix);
			suggestions.insert(suggestions.end(),
				nodeSuggestions.begin(), nodeSuggestions.end());
		}
	} else {
		for (auto& root : m_RootNodes) {
			if (!root->Matches(tokens[0])) {
				continue;
			}
			if (!source.HasPermission(root->GetRequiredPermissionLevel())) {
				continue;
			}
			CollectSuggestions(source, root, tokens,
				endsWithSpace ? tokens.size() : tokens.size() - 1,
				suggestions);
		}
	}

	std::sort(suggestions.begin(), suggestions.end());
	suggestions.erase(
		std::unique(suggestions.begin(), suggestions.end()),
		suggestions.end());
	return suggestions;
}

std::vector<std::string> CommandDispatcher::Tokenize(const std::string& input) {
	std::vector<std::string> tokens;
	std::istringstream stream(input);
	std::string token;
	while (stream >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

int CommandDispatcher::Execute(CommandSourceStack& source,
	const Ref<CommandNode>& node,
	const std::vector<std::string>& tokens,
	size_t index,
	std::unordered_map<std::string, std::string>& arguments) {

	if (index >= tokens.size()) {
		if (node->HasExecuteCallback()) {
			return node->GetExecuteCallback()(source, arguments);
		}
		return -1;
	}

	for (auto& child : node->GetChildren()) {
		if (!source.HasPermission(child->GetRequiredPermissionLevel())) {
			continue;
		}

		if (child->Matches(tokens[index])) {
			auto* argumentNode = dynamic_cast<const ArgumentNode*>(child.Raw());
			if (argumentNode) {
				arguments[argumentNode->GetName()] = tokens[index];
			}

			int result = Execute(source, child, tokens, index + 1, arguments);
			if (result != -1) {
				return result;
			}

			if (argumentNode) {
				arguments.erase(argumentNode->GetName());
			}
		}
	}

	if (node->HasExecuteCallback()) {
		return node->GetExecuteCallback()(source, arguments);
	}

	return -1;
}

void CommandDispatcher::CollectSuggestions(CommandSourceStack& source,
	const Ref<CommandNode>& node,
	const std::vector<std::string>& tokens,
	size_t index,
	std::vector<std::string>& output) {

	if (index <= 1) {
		std::string prefix = (index < tokens.size()) ? tokens[index] : "";
		for (auto& child : node->GetChildren()) {
			if (!source.HasPermission(child->GetRequiredPermissionLevel())) {
				continue;
			}
			auto childSuggestions = child->ListSuggestions(source, prefix);
			output.insert(output.end(),
				childSuggestions.begin(), childSuggestions.end());
		}
		return;
	}

	for (auto& child : node->GetChildren()) {
		if (index - 1 < tokens.size() && child->Matches(tokens[index - 1])) {
			CollectSuggestions(source, child, tokens, index - 1, output);
		}
	}
}

}
