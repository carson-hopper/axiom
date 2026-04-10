#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

/**
 * Brigadier-inspired command dispatcher.
 *
 * Commands are registered as trees of LiteralNode and
 * ArgumentNode objects. Dispatch walks the tree token by
 * token, collects parsed arguments, checks permissions,
 * and invokes the deepest matching execute callback.
 */
class CommandDispatcher {
public:
	/**
	 * Registers a top-level command literal and
	 * returns a reference so the caller can chain
	 * child nodes.
	 */
	CommandNode& Register(Ref<LiteralNode> literal);

	/**
	 * Parses the raw input and executes the matching
	 * command. Returns the integer result of the
	 * execute callback, or 0 when no command matched.
	 */
	int Dispatch(CommandSourceStack& source, const std::string& input);

	/**
	 * Returns tab-completion suggestions for the
	 * current partial input.
	 */
	std::vector<std::string> GetSuggestions(CommandSourceStack& source,
		const std::string& partial);

	/**
	 * Returns the root literal nodes.
	 */
	const std::vector<Ref<LiteralNode>>& GetRootNodes() const { return m_RootNodes; }

	/**
	 * Returns true when the source is allowed to run the given node.
	 * Checks the node's int permission level and string permission
	 * with OR semantics: passing either gate grants access.
	 */
	static bool CheckPermission(CommandSourceStack& source, const CommandNode& node);

private:
	/**
	 * Tokenizes the raw input string.
	 */
	static std::vector<std::string> Tokenize(const std::string& input);

	/**
	 * Recursively walks the tree to find an execute
	 * callback that matches the remaining tokens.
	 */
	int Execute(CommandSourceStack& source,
		const Ref<CommandNode>& node,
		const std::vector<std::string>& tokens,
		size_t index,
		std::unordered_map<std::string, std::string>& arguments);

	/**
	 * Recursively collects suggestions from child nodes.
	 */
	void CollectSuggestions(CommandSourceStack& source,
		const Ref<CommandNode>& node,
		const std::vector<std::string>& tokens,
		size_t index,
		std::vector<std::string>& output);

	std::vector<Ref<LiteralNode>> m_RootNodes;
};

}
