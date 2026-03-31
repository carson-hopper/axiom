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


public interface PluginContext {

    EventBus eventBus();

    ServiceRegistry services();

    PlayerManager players();

    BossBarManager bossBar();

    GuiManager guiManager();

    SidebarManager sidebarManager();

    /**
     * Register a command that plugins can use.
     * Commands must be annotated with @CommandMeta.
     */
    void registerCommand(Command command);

    /**
     * Get the plugin configuration.
     */
    PluginConfig config();

    /**
     * Get the plugin data folder.
     */
    File dataFolder();

    /**
     * Get the plugin logger.
     */
    Logger logger();
}
