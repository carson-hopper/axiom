package com.axiommc.plugin;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.EventPriority;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.plugin.Plugin;
import com.axiommc.api.plugin.PluginContext;
import com.axiommc.api.plugin.PluginSide;
import com.axiommc.plugin.command.AxiomCommand;
import com.axiommc.plugin.command.KillCommand;
import com.axiommc.plugin.command.TestConfigCommand;
import com.axiommc.plugin.command.TestGuiCommand;
import com.axiommc.plugin.command.TestPlayerCommand;
import com.axiommc.plugin.command.TestSidebarCommand;
import com.axiommc.plugin.command.TestWorldCommand;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@Plugin(id = "axiom", name = "Axiom", version = "1.0.0", side = PluginSide.SERVER)
public class AxiomPlugin extends com.axiommc.api.plugin.AxiomPlugin {

    private static final Logger logger = LoggerFactory.getLogger(AxiomPlugin.class);

    private static AxiomPlugin instance;

    @Override
    protected void onEnable(PluginContext context) {
        instance = this;

        context.registerCommand(new AxiomCommand());
        context.registerCommand(new KillCommand());
        context.registerCommand(new TestConfigCommand(context));
        context.registerCommand(new TestGuiCommand());
        context.registerCommand(new TestPlayerCommand());
        context.registerCommand(new TestSidebarCommand());
        context.registerCommand(new TestWorldCommand());

        EventBus eventBus = context.eventBus();
        eventBus.subscribe(PlayerJoinEvent.class, event -> {
            logger.info("{} joined", event.player().name());
        }, EventPriority.NORMAL);

        eventBus.subscribe(ServerStartEvent.class, event -> {
            logger.info("========================================");
            logger.info("{}: Server has started!", event.server().id());
            logger.info("The plugin event system is working correctly.");
            logger.info("========================================");
        }, EventPriority.NORMAL);

        logger.info("========================================");
        logger.info("Test Plugin loaded successfully!");
        logger.info("Plugin ID: test-plugin");
        logger.info("Plugin Version: 1.0.0");
        logger.info("========================================");
    }

    @Override
    public void onDisable() {
        logger.info("Test Plugin disabled");
    }

    public static AxiomPlugin instance() { return instance; }
    public PluginContext context() { return context; }

}
