#pragma once

#include "Axiom/Core/Base.h"

namespace Axiom {

	class EventBus;
	class CommandRegistry;
	class ServerConfig;

	class PluginContext {
	public:
		PluginContext(EventBus& eventBus, CommandRegistry& commandRegistry, ServerConfig& config)
			: m_EventBus(eventBus)
			, m_CommandRegistry(commandRegistry)
			, m_Config(config) {}

		EventBus& Events() { return m_EventBus; }
		CommandRegistry& Commands() { return m_CommandRegistry; }
		ServerConfig& Config() { return m_Config; }

	private:
		EventBus& m_EventBus;
		CommandRegistry& m_CommandRegistry;
		ServerConfig& m_Config;
	};

}
