#include "CorePlugin.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Plugin/PluginContext.h"
#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/CommandSender.h"

namespace Axiom {

	CorePlugin::CorePlugin() {
		m_Name = "Core";
		m_Version = "0.1.0";
	}

	void CorePlugin::OnEnable(PluginContext& context) {
		context.Commands().Register("stop", "Stop the server", [](CommandSender& sender, const std::vector<std::string>&) {
			sender.SendMessage("Stopping the server...");
		});

		context.Commands().Register("help", "List available commands", [&context](CommandSender& sender, const std::vector<std::string>&) {
			sender.SendMessage("Available commands:");
			for (const auto completions = context.Commands().TabComplete(sender, "");const auto& command : completions) {
				sender.SendMessage("  " + command);
			}
		});

		context.Commands().Register("version", "Show server version", [](CommandSender& sender, const std::vector<std::string>&) {
			sender.SendMessage("Axiom Server v0.1.0");
		});
	}

	void CorePlugin::OnDisable() {
		AX_CORE_INFO("CorePlugin disabled");
	}

}
