#include "PluginManager.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	void PluginManager::RegisterPlugin(Scope<Plugin> plugin) {
		AX_CORE_INFO("Registered plugin: {} v{}", plugin->Name(), plugin->Version());
		m_Plugins.push_back(std::move(plugin));
	}

	void PluginManager::EnableAll(PluginContext& context) {
		for (auto& plugin : m_Plugins) {
			AX_CORE_INFO("Enabling plugin: {}", plugin->Name());
			plugin->OnEnable(context);
		}
	}

	void PluginManager::DisableAll() {
		for (auto iterator = m_Plugins.rbegin(); iterator != m_Plugins.rend(); ++iterator) {
			AX_CORE_INFO("Disabling plugin: {}", (*iterator)->Name());
			(*iterator)->OnDisable();
		}
	}

	Plugin* PluginManager::FindPlugin(const std::string& name) {
		for (auto& plugin : m_Plugins) {
			if (plugin->Name() == name) {
				return plugin.get();
			}
		}
		return nullptr;
	}

}
