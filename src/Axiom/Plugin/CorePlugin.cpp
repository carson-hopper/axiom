#include "CorePlugin.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Plugin/PluginContext.h"
#include "Axiom/Command/CommandRegistry.h"
#include "../Command/Commands.h"

namespace Axiom {

	CorePlugin::CorePlugin() {
		m_Name = "Core";
		m_Version = "0.1.0";
	}

	void CorePlugin::OnEnable(PluginContext& context) {
		// Register built-in commands using their own classes
		context.Commands().Register(CreateRef<StopCommand>());
		context.Commands().Register(CreateRef<HelpCommand>(context.Commands()));

		AX_CORE_INFO("Core commands registered");
	}

	void CorePlugin::OnDisable() {
		AX_CORE_INFO("CorePlugin disabled");
	}

}
