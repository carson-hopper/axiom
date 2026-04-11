#pragma once

#include "Axiom/Command/Parsers/ArgumentParser.h"
#include "Axiom/Core/Base.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	class CommandSourceStack;

	/**
	 * A single node in the brigadier-style command tree.
	 *
	 * Nodes form a directed tree: each node may have children
	 * that represent the next token in the command line. Leaf
	 * nodes (or nodes with an execute callback) can terminate
	 * the parse and run the command.
	 */
	class CommandNode : public virtual RefCounted {
	public:
		using ExecuteCallback = std::function<int(CommandSourceStack&,
			const std::unordered_map<std::string, std::string>&)>;
		using SuggestionProvider = std::function<std::vector<std::string>(
			CommandSourceStack&, const std::string&)>;

		enum NodeType : uint8_t {
			Root     = 0x00,
			Literal  = 0x01,
			Argument = 0x02,
		};

		~CommandNode() override = default;

		/** Attempts to match the given token against this node's rule. */
		virtual bool Matches(const std::string& token) const = 0;

		/** Label or key name for this node. */
		virtual const std::string& GetName() const = 0;

		/** Usage text for help display. */
		virtual std::string GetUsageText() const = 0;

		/** The Brigadier node type (literal / argument). */
		virtual NodeType Type() const = 0;

		/** Generates suggestions for partial input. */
		virtual std::vector<std::string> ListSuggestions(
			CommandSourceStack& source, const std::string& partial) const = 0;

		/** Returns the Brigadier flags byte for the Commands packet. */
		uint8_t NodeFlags() const {
			uint8_t flags = static_cast<uint8_t>(Type());
			if (HasExecuteCallback()) flags |= 0x04;
			if (m_Redirect) flags |= 0x08;
			if (!m_SuggestionType.empty()) flags |= 0x10;
			return flags;
		}

		CommandNode& Then(const Ref<CommandNode>& child) {
			m_Children.push_back(child);
			return *child;
		}

		CommandNode& Executes(ExecuteCallback callback) {
			m_ExecuteCallback = std::move(callback);
			return *this;
		}

		CommandNode& Requires(const int permissionLevel) {
			m_RequiredPermissionLevel = permissionLevel;
			return *this;
		}

		CommandNode& Requires(const std::string& permission) {
			m_RequiredPermission = permission;
			return *this;
		}

		CommandNode& Redirect(const Ref<CommandNode>& target) {
			m_Redirect = target;
			return *this;
		}

		CommandNode& WithSuggestionType(const std::string& type) {
			m_SuggestionType = type;
			return *this;
		}

		bool HasExecuteCallback() const { return m_ExecuteCallback != nullptr; }
		const ExecuteCallback& GetExecuteCallback() const { return m_ExecuteCallback; }

		int GetRequiredPermissionLevel() const { return m_RequiredPermissionLevel; }
		const std::string& GetRequiredPermission() const { return m_RequiredPermission; }

		const std::vector<Ref<CommandNode>>& GetChildren() const { return m_Children; }
		const Ref<CommandNode>& GetRedirect() const { return m_Redirect; }
		const std::string& GetSuggestionType() const { return m_SuggestionType; }

	protected:
		std::vector<Ref<CommandNode>> m_Children;
		ExecuteCallback m_ExecuteCallback;
		Ref<CommandNode> m_Redirect;
		std::string m_SuggestionType;
		std::string m_RequiredPermission;
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

		NodeType Type() const override { return NodeType::Literal; }

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
		ArgumentNode(std::string name, Ref<ArgumentParser> parser)
			: m_Name(std::move(name)), m_Parser(std::move(parser)) {}

		bool Matches(const std::string& /*token*/) const override {
			return true;
		}

		const std::string& GetName() const override { return m_Name; }

		std::string GetUsageText() const override { return "<" + m_Name + ">"; }

		NodeType Type() const override { return NodeType::Argument; }

		const Ref<ArgumentParser>& GetParser() const { return m_Parser; }

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
		Ref<ArgumentParser> m_Parser;
		SuggestionProvider m_SuggestionProvider;
	};

	/** Factory: Create a literal node. */
	inline Ref<LiteralNode> Literal(const std::string& name) {
		return Ref<LiteralNode>::Create(name);
	}

	/** Factory: Create an argument node with a parser. */
	inline Ref<ArgumentNode> Argument(const std::string& name, Ref<ArgumentParser> parser) {
		return Ref<ArgumentNode>::Create(name, std::move(parser));
	}

}
