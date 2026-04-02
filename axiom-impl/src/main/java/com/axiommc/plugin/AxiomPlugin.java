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
import com.axiommc.plugin.command.*;
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
        context.registerCommand(new ScreenCommand());

        context.registerCommand(new TestConfigCommand(context));
        context.registerCommand(new TestGuiCommand());
        context.registerCommand(new TestPlayerCommand());
        context.registerCommand(new TestSidebarCommand());
        context.registerCommand(new TestWorldCommand());

        EventBus eventBus = context.eventBus();
        eventBus.subscribe(PlayerJoinEvent.class, event -> {
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
