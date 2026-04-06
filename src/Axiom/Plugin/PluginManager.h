#pragma once

#include "Axiom/Plugin/Plugin.h"
#include "Axiom/Plugin/PluginContext.h"

#include <string>
#include <vector>

namespace Axiom {

	class PluginManager {
	public:
		void RegisterPlugin(Scope<Plugin> plugin);
		void EnableAll(PluginContext& context);
		void DisableAll();
		Plugin* FindPlugin(const std::string& name);

	private:
		std::vector<Scope<Plugin>> m_Plugins;
	};

}
