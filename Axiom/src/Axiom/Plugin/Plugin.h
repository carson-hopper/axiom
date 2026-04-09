#pragma once

#include <string>

namespace Axiom {

	class PluginContext;

	/**
	 * Base class for all Axiom server plugins.
	 *
	 * Plugins extend the server's functionality by responding to events,
	 * registering commands, and interacting with the world. Plugins have
	 * a defined lifecycle: registered -> enabled -> disabled.
	 *
	 * To create a plugin:
	 * 1. Subclass Plugin
	 * 2. Set m_Name and m_Version in constructor
	 * 3. Implement OnEnable() for initialization
	 * 4. Implement OnDisable() for cleanup
	 * 5. Register event handlers and commands in OnEnable()
	 *
	 * Example:
	 * @code
	 * class MyPlugin : public Plugin {
	 * public:
	 *     MyPlugin() {
	 *         m_Name = "MyPlugin";
	 *         m_Version = "1.0.0";
	 *     }
	 *
	 *     void OnEnable(PluginContext& context) override {
	 *         // Register events, commands
	 *     }
	 *
	 *     void OnDisable() override {
	 *         // Cleanup
	 *     }
	 * };
	 * @endcode
	 */
	class Plugin {
	public:
		virtual ~Plugin() = default;

		/**
		 * Called when the plugin is enabled.
		 *
		 * This is where you should register event handlers, commands, and
		 * perform any initialization. The plugin context provides access to
		 * server APIs like the event bus and command registry.
		 *
		 * @param context Provides access to server APIs. Valid only during this call.
		 *
		 * @note Exceptions thrown here will be caught and logged, but the plugin
		 *       will be marked as enabled. Ensure cleanup in OnDisable().
		 */
		virtual void OnEnable(PluginContext& context) = 0;

		/**
		 * Called when the plugin is disabled.
		 *
		 * This is where you should clean up resources, unregister handlers,
		 * and save persistent data. This method is called even if OnEnable()
		 * threw an exception.
		 *
		 * @note This method should not throw exceptions. All cleanup must succeed
		 *       or be silently logged.
		 */
		virtual void OnDisable() = 0;

		/**
		 * Gets the plugin name.
		 *
		 * @return The plugin identifier used in logging and dependency resolution.
		 */
		const std::string& Name() const { return m_Name; }

		/**
		 * Gets the plugin version string.
		 *
		 * @return Version in any format (e.g., "1.0.0", "2.1.0-beta").
		 */
		const std::string& Version() const { return m_Version; }

	protected:
		std::string m_Name;    ///< Plugin identifier (must be unique)
		std::string m_Version; ///< Human-readable version string
	};

}
