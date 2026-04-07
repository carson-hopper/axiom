#pragma once

#include "Axiom/Plugin/Plugin.h"
#include "Axiom/Plugin/PluginContext.h"

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Manages the lifecycle of all plugins in the server.
	 *
	 * The PluginManager is responsible for registering, enabling, and disabling
	 * plugins. Plugins are stored in registration order and disabled in reverse
	 * order to respect dependency chains.
	 *
	 * This class is NOT thread-safe. All methods should be called from the main
	 * server thread only.
	 *
	 * Example usage:
	 * @code
	 * PluginManager manager;
	 * manager.RegisterPlugin(CreateScope<MyPlugin>());
	 * manager.EnableAll(context);
	 * // ... server runs ...
	 * manager.DisableAll();
	 * @endcode
	 */
	class PluginManager {
	public:
		/**
		 * Registers a plugin without enabling it.
		 *
		 * The plugin is moved into internal storage. Ownership is transferred
		 * to the PluginManager. The plugin will not receive OnEnable() until
		 * EnableAll() is called.
		 *
		 * @param plugin The plugin to register. Must not be nullptr.
		 */
		void RegisterPlugin(Scope<Plugin> plugin);

		/**
		 * Enables all registered plugins in registration order.
		 *
		 * Each plugin's OnEnable() method is called with the provided context.
		 * If a plugin throws during enable, subsequent plugins are still enabled.
		 *
		 * @param context The plugin context providing server APIs and event bus.
		 */
		void EnableAll(PluginContext& context);

		/**
		 * Disables all enabled plugins in reverse registration order.
		 *
		 * Each plugin's OnDisable() method is called. This ensures that
		 * dependent plugins are disabled before their dependencies.
		 *
		 * Plugins are disabled regardless of whether they threw during enable.
		 */
		void DisableAll();

		/**
		 * Finds a plugin by its registered name.
		 *
		 * @param name The plugin name to search for (case-sensitive).
		 * @return Pointer to the plugin if found, nullptr otherwise.
		 */
		Plugin* FindPlugin(const std::string& name);

		/**
		 * Gets the number of registered plugins.
		 *
		 * @return The total count of registered plugins (enabled or not).
		 */
		size_t PluginCount() const { return m_Plugins.size(); }

	private:
		std::vector<Scope<Plugin>> m_Plugins;
	};

}
