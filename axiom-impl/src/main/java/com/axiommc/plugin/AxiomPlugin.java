package com.axiommc.plugin;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.EventPriority;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.plugin.Plugin;
import com.axiommc.api.plugin.PluginContext;
import com.axiommc.api.plugin.PluginSide;
import com.axiommc.fabric.Axiom;
import com.axiommc.plugin.command.AxiomCommand;
import com.axiommc.plugin.command.player.GameModeCommand;
import com.axiommc.plugin.command.RandomTeleportCommand;
import com.axiommc.plugin.command.ScreenCommand;
import com.axiommc.plugin.command.player.TeleportCommand;
import com.axiommc.plugin.command.TestConfigCommand;
import com.axiommc.plugin.command.TestGuiCommand;
import com.axiommc.plugin.command.TestPlayerCommand;
import com.axiommc.plugin.command.TestSidebarCommand;
import com.axiommc.plugin.command.TestWorldCommand;
import com.axiommc.plugin.command.player.KillCommand;

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
        context.registerCommand(new GameModeCommand());
        context.registerCommand(new ScreenCommand());

        context.registerCommand(new TestConfigCommand(context));
        context.registerCommand(new TestGuiCommand());
        context.registerCommand(new TestPlayerCommand());
        context.registerCommand(new TestSidebarCommand());
        context.registerCommand(new TestWorldCommand());

        EventBus eventBus = context.eventBus();
        eventBus.subscribe(PlayerJoinEvent.Init.class, event -> {
            Axiom.logger().info(ChatComponent.textf("{} <{}> init!", event.username(), event.uuid()).color(ChatColor.BLUE));
        }, EventPriority.NORMAL);
        eventBus.subscribe(PlayerJoinEvent.Pre.class, event -> {
            Axiom.logger().info(ChatComponent.textf("{} pre connecting!", event.player().name()).color(ChatColor.BLUE));
        }, EventPriority.NORMAL);
        eventBus.subscribe(PlayerJoinEvent.Connecting.class, event -> {
            event.player().sendMessage(ChatComponent.textf("{} connecting!", event.player().name()).color(ChatColor.BLUE));
            Axiom.logger().info(ChatComponent.textf("{} connecting!", event.player().name()).color(ChatColor.BLUE));
        }, EventPriority.NORMAL);
        eventBus.subscribe(PlayerJoinEvent.Post.class, event -> {
            event.player().sendMessage(ChatComponent.textf("{} joined!", event.player().name()).color(ChatColor.BLUE));
            Axiom.logger().info(ChatComponent.textf("{} joined!", event.player().name()).color(ChatColor.BLUE));
        }, EventPriority.NORMAL);
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
