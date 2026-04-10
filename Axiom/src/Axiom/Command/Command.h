#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Command/CommandSender.h"

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Abstract base class for server commands.
	 *
	 * Commands represent actions that can be executed by players or the console.
	 * Each command has a name, description, and execution logic. Commands can
	 * also provide tab completion for their arguments.
	 *
	 * To implement a command:
	 * 1. Subclass Command
	 * 2. Implement Name(), Description(), and Execute()
	 * 3. Optionally override TabComplete() for argument completion
	 * 4. Register with CommandRegistry
	 *
	 * Example:
	 * @code
	 * class TeleportCommand : public Command {
	 * public:
	 *     const std::string& Name() const override {
	 *         static std::string name = "teleport";
	 *         return name;
	 *     }
	 *
	 *     const std::string& Description() const override {
	 *         static std::string desc = "Teleports a player to another location";
	 *         return desc;
	 *     }
	 *
	 *     void Execute(CommandSender& sender, const std::vector<std::string>& args) override {
	 *         if (args.size() < 3) {
	 *             sender.SendMessage(ChatComponent::Create()
	 *                 .Text("Usage: /teleport <x> <y> <z>")
	 *                 .Color(ChatColor::Red)
	 *                 .Build());
	 *             return;
	 *         }
	 *         // ... teleport logic ...
	 *     }
	 * };
	 * @endcode
	 */
	class Command : public virtual RefCounted {
	public:
		virtual ~Command() = default;

		/**
		 * Gets the command name (what users type, e.g., "give").
		 *
		 * @return The command identifier. Must be unique in the registry.
		 */
		virtual const std::string& Name() const = 0;

		/**
		 * Gets a brief description of what the command does.
		 *
		 * @return Human-readable description for help listings.
		 */
		virtual const std::string& Description() const = 0;

		/**
		 * Executes the command.
		 *
		 * Called when a player or console invokes this command. The arguments
		 * vector contains all space-separated tokens after the command name.
		 *
		 * @param sender The entity executing the command (player or console).
		 * @param arguments Tokenized command arguments (may be empty).
		 *
		 * @note This runs on the main server thread. Avoid blocking operations.
		 */
		virtual void Execute(CommandSender& sender, const std::vector<std::string>& arguments) = 0;

		/**
		 * Provides tab completions for command arguments.
		 *
		 * Called when the user presses TAB while typing this command.
		 * Return a list of possible completions based on current input.
		 *
		 * @param sender The entity requesting completions (for permission checks).
		 * @param arguments Current arguments typed so far.
		 * @return List of possible completions for the current argument position.
		 *
		 * @note Default implementation returns empty list (no completions).
		 */
		virtual std::vector<std::string> TabComplete(
			CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {
			return {};
		}
	};

}
