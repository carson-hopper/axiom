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

		EventBus& Events() const { return m_EventBus; }
		CommandRegistry& Commands() const { return m_CommandRegistry; }
		ServerConfig& Config() const { return m_Config; }

	private:
		EventBus& m_EventBus;
		CommandRegistry& m_CommandRegistry;
		ServerConfig& m_Config;
	};

}
