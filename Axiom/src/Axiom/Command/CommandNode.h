#pragma once

#include "Axiom/Core/Base.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

class CommandSourceStack;

/**
 * Flags describing how a command argument is parsed.
 */
enum class ArgumentType {
	String,
	Integer,
	Float,
	Boolean,
	Entity,
	GameMode,
	BlockPosition
};

/**
 * A single node in the brigadier-style command tree.
 *
 * Nodes form a directed tree: each node may have children
 * that represent the next token in the command line. Leaf
 * nodes (or nodes with an execute callback) can terminate
 * the parse and run the command.
 */
class CommandNode {
public:
	using ExecuteCallback = std::function<int(CommandSourceStack&, const std::unordered_map<std::string, std::string>&)>;
	using SuggestionProvider = std::function<std::vector<std::string>(CommandSourceStack&, const std::string&)>;

	virtual ~CommandNode() = default;

	/**
	 * Attempts to parse the given token against
	 * this node's matching rule.
	 */
	virtual bool Matches(const std::string& token) const = 0;

	/**
	 * Returns the name or label for this node.
	 */
	virtual const std::string& GetName() const = 0;

	/**
	 * Returns usage text for help display.
	 */
	virtual std::string GetUsageText() const = 0;

	/**
	 * Generates suggestions for partial input.
	 */
	virtual std::vector<std::string> ListSuggestions(
		CommandSourceStack& source, const std::string& partial) const = 0;

	CommandNode& Then(Ref<CommandNode> child) {
		m_Children.push_back(child);
		return *child;
	}

	CommandNode& Executes(ExecuteCallback callback) {
		m_ExecuteCallback = std::move(callback);
		return *this;
	}

	CommandNode& Requires(int permissionLevel) {
		m_RequiredPermissionLevel = permissionLevel;
		return *this;
	}

	bool HasExecuteCallback() const { return m_ExecuteCallback != nullptr; }
	const ExecuteCallback& GetExecuteCallback() const { return m_ExecuteCallback; }

	int GetRequiredPermissionLevel() const { return m_RequiredPermissionLevel; }

	const std::vector<Ref<CommandNode>>& GetChildren() const { return m_Children; }

protected:
	std::vector<Ref<CommandNode>> m_Children;
	ExecuteCallback m_ExecuteCallback;
	int m_RequiredPermissionLevel = 0;
};

/**
 * A literal node matches a fixed keyword exactly.
 * Used for command names and sub-command labels
 * (e.g. "time", "set", "add").
 */
class LiteralNode : public CommandNode {
public:
	explicit LiteralNode(std::string literal)
		: m_Literal(std::move(literal)) {}

	bool Matches(const std::string& token) const override {
		return token == m_Literal;
	}

	const std::string& GetName() const override { return m_Literal; }

	std::string GetUsageText() const override { return m_Literal; }

	std::vector<std::string> ListSuggestions(
		CommandSourceStack& /*source*/, const std::string& partial) const override {
		if (m_Literal.find(partial) == 0) {
			return {m_Literal};
		}
		return {};
	}

private:
	std::string m_Literal;
};

/**
 * An argument node captures a user-supplied value.
 * The node name becomes the key in the parsed arguments
 * map (e.g. "target", "amount", "x").
 */
class ArgumentNode : public CommandNode {
public:
	ArgumentNode(std::string name, ArgumentType type)
		: m_Name(std::move(name)), m_Type(type) {}

	bool Matches(const std::string& /*token*/) const override {
		return true;
	}

	const std::string& GetName() const override { return m_Name; }

	std::string GetUsageText() const override { return "<" + m_Name + ">"; }

	ArgumentType GetArgumentType() const { return m_Type; }

	ArgumentNode& WithSuggestions(SuggestionProvider provider) {
		m_SuggestionProvider = std::move(provider);
		return *this;
	}

	std::vector<std::string> ListSuggestions(
		CommandSourceStack& source, const std::string& partial) const override {
		if (m_SuggestionProvider) {
			return m_SuggestionProvider(source, partial);
		}
		return {};
	}

private:
	std::string m_Name;
	ArgumentType m_Type;
	SuggestionProvider m_SuggestionProvider;
};

/**
 * Factory functions for concise tree construction.
 */
inline Ref<LiteralNode> Literal(const std::string& name) {
	return CreateRef<LiteralNode>(name);
}

inline Ref<ArgumentNode> Argument(const std::string& name, ArgumentType type) {
	return CreateRef<ArgumentNode>(name, type);
}

}
