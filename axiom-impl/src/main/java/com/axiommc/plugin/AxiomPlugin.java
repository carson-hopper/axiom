package com.axiommc.plugin;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.EventPriority;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.plugin.Plugin;
import com.axiommc.api.plugin.PluginContext;
import com.axiommc.api.plugin.PluginSide;
import com.axiommc.fabric.Axiom;
import com.axiommc.plugin.command.AxiomCommand;
import com.axiommc.plugin.command.KillCommand;
import com.axiommc.plugin.command.RandomTeleportCommand;
import com.axiommc.plugin.command.TeleportCommand;
import com.axiommc.plugin.command.TestConfigCommand;
import com.axiommc.plugin.command.TestGuiCommand;
import com.axiommc.plugin.command.TestPlayerCommand;
import com.axiommc.plugin.command.TestSidebarCommand;
import com.axiommc.plugin.command.TestWorldCommand;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@Plugin(id = "axiom", name = "Axiom", version = "1.0.0", side = PluginSide.SERVER)
public class AxiomPlugin extends com.axiommc.api.plugin.AxiomPlugin {

    private static AxiomPlugin instance;

    @Override
    protected void onEnable(PluginContext context) {
        instance = this;

        context.registerCommand(new AxiomCommand());
        context.registerCommand(new KillCommand());
        context.registerCommand(new RandomTeleportCommand(context));
        context.registerCommand(new TeleportCommand());
        context.registerCommand(new TestConfigCommand(context));
        context.registerCommand(new TestGuiCommand());
        context.registerCommand(new TestPlayerCommand());
        context.registerCommand(new TestSidebarCommand());
        context.registerCommand(new TestWorldCommand());

        EventBus eventBus = context.eventBus();
        eventBus.subscribe(PlayerJoinEvent.class, event -> {
            event.player().sendMessage("Yayy");
            Axiom.logger().info(ChatComponent.textf("%s joined!", event.player().name()).color(ChatColor.BLUE));
        }, EventPriority.NORMAL);

        eventBus.subscribe(ServerStartEvent.class, event -> {
            Axiom.logger().info("========================================");
            Axiom.logger().info("{}: Server has started!", event.server().id());
            Axiom.logger().info("The plugin event system is working correctly.");
            Axiom.logger().info("========================================");
        }, EventPriority.NORMAL);

        Axiom.logger().info("========================================");
        Axiom.logger().info("Test Plugin loaded successfully!");
        Axiom.logger().info("Plugin ID: test-plugin");
        Axiom.logger().info("Plugin Version: 1.0.0");
        Axiom.logger().info("========================================");
    }

    @Override
    public void onDisable() {
        Axiom.logger().info("Test Plugin disabled");
    }

    public static AxiomPlugin instance() {
        return instance;
    }

    public PluginContext context() {
        return context;
    }

}
