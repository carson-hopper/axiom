#include "axpch.h"
#include "Axiom/Command/Commands/HelpCommand.h"

#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

	HelpCommand::HelpCommand(CommandRegistry& registry)
		: m_Registry(registry) {
	}

	void HelpCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
		if (!sender.HasPermission(Permission())) {
			sender.SendMessage(ChatComponent::Create()
				.Text("You don't have permission to view help")
				.Color(ChatColor::Red)
				.Build());
			return;
		}

		if (arguments.empty()) {
			ShowGeneralHelp(sender);
		} else {
			ShowCommandHelp(sender, arguments[0]);
		}
	}

	void HelpCommand::ShowGeneralHelp(CommandSender& sender) {
		sender.SendMessage(ChatComponent::Create()
			.Text("=== ")
			.Append("Available Commands", ChatColor::Gold)
			.Append(" ===")
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("Use ")
			.Append("/help <command>", ChatColor::Yellow)
			.Append(" for detailed info")
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("")
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("/stop ")
			.Append("- Stop the server", ChatColor::Gray)
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("/help ")
			.Append("- Show this help message", ChatColor::Gray)
			.Build());
	}

	void HelpCommand::ShowCommandHelp(CommandSender& sender, const std::string& commandName) {
		if (!m_Registry.HasCommand(commandName)) {
			sender.SendMessage(ChatComponent::Create()
				.Text("Unknown command: ")
				.Append(commandName, ChatColor::Red)
				.Build());
			return;
		}

		sender.SendMessage(ChatComponent::Create()
			.Text("=== Command: ")
			.Append(commandName, ChatColor::Gold)
			.Append(" ===")
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("Usage: /" + commandName)
			.Color(ChatColor::Yellow)
			.Build());

		sender.SendMessage(ChatComponent::Create()
			.Text("Description: See /help for details")
			.Color(ChatColor::Gray)
			.Build());
	}

	std::vector<std::string> HelpCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
		if (arguments.size() <= 1) {
			// Return list of available commands
			return {"stop", "help"};
		}
		return {};
	}

}
