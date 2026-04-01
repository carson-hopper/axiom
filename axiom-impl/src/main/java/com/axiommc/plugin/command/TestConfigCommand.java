package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Greedy;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.config.PluginConfig;
import com.axiommc.api.plugin.PluginContext;

@CommandMeta(
        name = "testconfig",
        description = "Test plugin config and context APIs",
        permission = "axiom.test"
)
public class TestConfigCommand implements Command {

    private final PluginContext context;

    public TestConfigCommand(PluginContext context) {
        this.context = context;
    }

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /testconfig <info|get|set|reload>").color(ChatColor.RED));
    }

    @Subcommand
    public void info(CommandSender sender) {
        if (context == null) {
            sender.sendMessage(ChatComponent.text("Plugin context not available").color(ChatColor.RED));
            return;
        }
        sender.sendMessage(ChatComponent.text("Plugin Context Info:").color(ChatColor.GOLD));
        sender.sendMessage(ChatComponent.text("  Data Folder: " + context.dataFolder().getAbsolutePath())
                .color(ChatColor.GRAY));

        PluginConfig config = context.config();
        sender.sendMessage(ChatComponent.text("  Config File: " + config.getClass().getSimpleName())
                .color(ChatColor.GRAY));
    }

    @Subcommand
    public void get(CommandSender sender, @Arg("key") String key) {
        PluginConfig config = context.config();
        if (!config.contains(key)) {
            sender.sendMessage(ChatComponent.text("Key '" + key + "' not found").color(ChatColor.RED));
            return;
        }

        String value = config.getString(key, "");
        sender.sendMessage(ChatComponent.text("Config[" + key + "] = " + value).color(ChatColor.GREEN));
    }

    @Subcommand
    public void set(CommandSender sender, @Arg("key") String key, @Greedy String value) {
        PluginConfig config = context.config();
        config.set(key, value);
        config.save();
        sender.sendMessage(ChatComponent.text("Set " + key + " = " + value).color(ChatColor.GREEN));
    }

    @Subcommand
    public void reload(CommandSender sender) {
        context.config().reload();
        sender.sendMessage(ChatComponent.text("Config reloaded").color(ChatColor.GREEN));
    }

}
