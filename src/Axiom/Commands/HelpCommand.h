#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

class CommandRegistry;

/**
 * Lists available commands and provides command help.
 *
 * Usage: /help [command]
 * Permission: server.help
 */
class HelpCommand : public Command {
public:
	explicit HelpCommand(CommandRegistry& registry);

	const std::string& Name() const override {
		static std::string name = "help";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "List available commands";
		return desc;
	}

	const std::string& Usage() const {
		static std::string usage = "/help [command]";
		return usage;
	}

	const std::string& Permission() const {
		static std::string perm = "server.help";
		return perm;
	}

	void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

	std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;

private:
	void ShowGeneralHelp(CommandSender& sender);
	void ShowCommandHelp(CommandSender& sender, const std::string& commandName);

private:
	CommandRegistry& m_Registry;
};

}
