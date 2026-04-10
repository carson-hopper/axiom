#pragma once

#include "Axiom/Command/Command.h"
#include "Axiom/Command/CommandDispatcher.h"
#include "Axiom/Command/CommandSourceStack.h"

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Central registry for all server commands.
	 *
	 * Wraps CommandDispatcher: when a Command is registered, its BuildTree()
	 * is called to get a LiteralNode which is added to the dispatcher's
	 * root nodes. Dispatch and TabComplete then walk the Brigadier tree.
	 *
	 * Thread safety: NOT thread-safe. Register commands during server
	 * initialization or from the main thread only. Dispatch should only
	 * be called from the main thread.
	 */
	class CommandRegistry {
	public:
		/**
		 * Register a command. The command's BuildTree() is called once
		 * and the resulting node is added to the dispatcher.
		 */
		void Register(Ref<Command> command);

		/**
		 * Parse and execute a command string.
		 */
		bool Dispatch(CommandSourceStack& source, const std::string& input);

		/**
		 * Generate tab completions for a partial command line.
		 */
		std::vector<std::string> TabComplete(CommandSourceStack& source,
			const std::string& partial);

		/**
		 * Unregister a command by name.
		 */
		void Unregister(const std::string& name);

		/**
		 * Check if a command is registered.
		 */
		bool HasCommand(const std::string& name) const;

		/**
		 * Total number of registered commands.
		 */
		size_t CommandCount() const { return m_Commands.size(); }

		/**
		 * Returns all registered command names.
		 */
		std::vector<std::string> GetCommandNames() const;

		/**
		 * Returns the registered command objects.
		 */
		const std::vector<Ref<Command>>& GetCommands() const { return m_Commands; }

		/**
		 * Returns the root nodes of the underlying Brigadier tree for
		 * network serialization via the Commands packet.
		 */
		const std::vector<Ref<LiteralNode>>& GetRootNodes() const {
			return m_Dispatcher.GetRootNodes();
		}

	private:
		std::vector<Ref<Command>> m_Commands;
		CommandDispatcher m_Dispatcher;
	};

}
