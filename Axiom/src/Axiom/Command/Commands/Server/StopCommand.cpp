#include "axpch.h"
#include "Axiom/Command/Commands/Server/StopCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

	void StopCommand::Execute(CommandSender& sender, const std::vector<std::string>& /*arguments*/) {
		if (!sender.HasPermission(Permission())) {
			sender.SendMessage(ChatComponent::Create()
				.Text("You don't have permission to stop the server")
				.Color(ChatColor::Red)
				.Build());
			return;
		}

		sender.SendMessage(ChatComponent::Create()
			.Text("Stopping the server...")
				.Color(ChatColor::Yellow)
				.Build());

		AX_CORE_INFO("Server stop requested by {}", sender.Name());
		
		Application::Instance().Stop();
	}

	std::vector<std::string> StopCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {
		// Stop command takes no arguments
		return {};
	}

}
