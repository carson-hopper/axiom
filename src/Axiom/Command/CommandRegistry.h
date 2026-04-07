#pragma once

#include "Axiom/Command/Command.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Callback type for lambda-based command registration.
	 *
	 * @param sender The entity executing the command (player or console).
	 * @param arguments Tokenized command arguments (excludes command name).
	 */
	using CommandCallback = std::function<void(CommandSender&, const std::vector<std::string>&)>;

	/**
	 * Central registry for all server commands.
	 *
	 * The CommandRegistry manages command registration, dispatch, and tab completion.
	 * Commands can be registered either as full Command objects or as simple lambdas.
	 *
	 * Thread safety: This class is NOT thread-safe. Register commands during
	 * server initialization or from the main thread only. Dispatch should only
	 * be called from the main thread.
	 *
	 * Example usage:
	 * @code
	 * // Lambda style (simple commands)
	 * registry.Register("hello", "Says hello", [](CommandSender& sender, auto& args) {
	 *     sender.SendMessage("Hello, world!");
	 * });
	 *
	 * // Class style (complex commands with tab completion)
	 * registry.Register(CreateRef<MyComplexCommand>());
	 * @endcode
	 */
	class CommandRegistry {
	public:
		/**
		 * Registers a command from a Command object.
		 *
		 * @param command The command to register. Must have a unique name.
		 * @note If a command with the same name exists, it will be overwritten.
		 */
		void Register(Ref<Command> command);

		/**
		 * Registers a simple command using a lambda callback.
		 *
		 * This is a convenience method for commands that don't need custom
		 * tab completion. For full tab completion support, create a Command subclass.
		 *
		 * @param name The command name (what users type, e.g., "help").
		 * @param description Brief description shown in command listings.
		 * @param callback The function to execute when command is invoked.
		 */
		void Register(const std::string& name, const std::string& description, CommandCallback callback);

		/**
		 * Parses and executes a command string.
		 *
		 * The input is tokenized by whitespace. The first token is the command
		 * name; remaining tokens are passed as arguments.
		 *
		 * @param sender The entity executing the command.
		 * @param input The full command line (e.g., "give @s diamond 64").
		 * @return true if command was found and executed, false if unknown.
		 */
		bool Dispatch(CommandSender& sender, const std::string& input);

		/**
		 * Generates tab completions for a partial command.
		 *
		 * If completing the command name, returns all matching commands.
		 * If completing arguments, delegates to the command's TabComplete method.
		 *
		 * @param sender The entity requesting completion (for permission checks).
		 * @param partial The partial input (e.g., "gi" or "give @s dia").
		 * @return Sorted list of possible completions.
		 */
		std::vector<std::string> TabComplete(CommandSender& sender, const std::string& partial);

		/**
		 * Removes a command from the registry.
		 *
		 * @param name The command name to unregister.
		 * @note Safe to call even if command doesn't exist.
		 */
		void Unregister(const std::string& name);

		/**
		 * Checks if a command is registered.
		 *
		 * @param name The command name to check.
		 * @return true if the command exists in the registry.
		 */
		bool HasCommand(const std::string& name) const {
			return m_Commands.find(name) != m_Commands.end();
		}

		/**
		 * Gets the number of registered commands.
		 *
		 * @return Total count of registered commands.
		 */
		size_t CommandCount() const { return m_Commands.size(); }

	private:
		std::unordered_map<std::string, Ref<Command>> m_Commands;
	};

}
