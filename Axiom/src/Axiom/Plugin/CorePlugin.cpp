#include "CorePlugin.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Plugin/PluginContext.h"
#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/Commands.h"

namespace Axiom {

	CorePlugin::CorePlugin() {
		m_Name = "Core";
		m_Version = "0.1.0";
	}

	void CorePlugin::OnEnable(PluginContext& context) {
		// Register built-in commands using their own classes
        context.Commands().Register(Ref<StopCommand>::Create());
        context.Commands().Register(Ref<OpCommand>::Create());
        context.Commands().Register(Ref<DeopCommand>::Create());

#if AX_DEBUG
        context.Commands().Register(Ref<RestartCommand>::Create());
#endif

        context.Commands().Register(Ref<GamemodeCommand>::Create());
		context.Commands().Register(Ref<DifficultyCommand>::Create());
		context.Commands().Register(Ref<TimeCommand>::Create());
		context.Commands().Register(Ref<WeatherCommand>::Create());
		context.Commands().Register(Ref<TeleportCommand>::Create());
		context.Commands().Register(Ref<SetWorldSpawnCommand>::Create());
		context.Commands().Register(Ref<GameruleCommand>::Create());
		context.Commands().Register(Ref<ListCommand>::Create());

        context.Commands().Register(Ref<HelpCommand>::Create(context.Commands()));
	}

	void CorePlugin::OnDisable() {
		AX_CORE_INFO("CorePlugin disabled");
	}

}
