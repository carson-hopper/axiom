package com.axiommc.api.plugin;

import com.axiommc.api.command.Command;
import com.axiommc.api.config.PluginConfig;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.PlayerManager;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.BossBarManager;
import org.slf4j.Logger;

import java.io.File;


/**
 * Provides access to plugin-related services and managers.
 *
 * <p>A plugin context is passed to plugins on initialization and provides
 * access to core services such as the event bus, player manager, GUIs,
 * and configuration.
 */
public interface PluginContext {

    /**
     * Gets the event bus for subscribing to events.
     *
     * @return the event bus
     */
    EventBus eventBus();

    /**
     * Gets the service registry for accessing shared services.
     *
     * @return the service registry
     */
    ServiceRegistry services();

    /**
     * Gets the player manager.
     *
     * @return the player manager
     */
    PlayerManager players();

    /**
     * Gets the boss bar manager.
     *
     * @return the boss bar manager
     */
    BossBarManager bossBar();

    /**
     * Gets the GUI manager.
     *
     * @return the GUI manager
     */
    GuiManager guiManager();

    /**
     * Gets the sidebar manager.
     *
     * @return the sidebar manager
     */
    SidebarManager sidebarManager();

    /**
     * Registers a command for this plugin.
     *
     * <p>The command class must be annotated with {@code @CommandMeta}.
     *
     * @param command the command to register (must not be null)
     */
    void registerCommand(Command command);

    /**
     * Gets the plugin configuration.
     *
     * @return the plugin config
     */
    PluginConfig config();

    /**
     * Gets the plugin's data folder.
     *
     * <p>This folder is used for storing plugin files and configurations.
     * It is created automatically if it does not exist.
     *
     * @return the plugin data directory
     */
    File dataFolder();

    /**
     * Gets the plugin logger.
     *
     * @return the SLF4J logger for this plugin
     */
    Logger logger();
}
