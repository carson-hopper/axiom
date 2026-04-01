package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.player.Player;
import com.axiommc.api.sidebar.Sidebar;
import com.axiommc.fabric.Axiom;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

@CommandMeta(
        name = "testsidebar",
        description = "Test sidebar API",
        permission = "axiom.test"
)
public class TestSidebarCommand implements Command {

    private static final Map<String, Sidebar> sidebars = new HashMap<>();

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /testsidebar <create|update|hide>").color(ChatColor.RED));
    }

    @Subcommand
    public void create(CommandSender sender) {
        Optional<Player> player = Axiom.players().stream()
                .map(p -> (Player) p)
                .findFirst();
        if (player.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No players online").color(ChatColor.RED));
            return;
        }

        Player p = player.get();
        Sidebar sidebar = Axiom.sidebarManager().create(ChatComponent.text("Test Board").color(ChatColor.GOLD));

        for (int i = 0; i < 5; i++) {
            sidebar.line(i, ChatComponent.text("Line " + (i + 1)).color(ChatColor.WHITE));
        }

        sidebar.show(p);
        sidebars.put(p.name(), sidebar);
        sender.sendMessage(ChatComponent.text("Created sidebar for " + p.name()).color(ChatColor.GREEN));
    }

    @Subcommand
    public void update(CommandSender sender) {
        Optional<Player> player = Axiom.players().stream()
                .map(p -> (Player) p)
                .findFirst();
        if (player.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No players online").color(ChatColor.RED));
            return;
        }

        Player p = player.get();
        Sidebar sidebar = sidebars.get(p.name());
        if (sidebar == null) {
            sender.sendMessage(ChatComponent.text("No sidebar for " + p.name()).color(ChatColor.RED));
            return;
        }

        sidebar.title(ChatComponent.text("Updated!").color(ChatColor.YELLOW));
        sidebar.line(0, ChatComponent.text("Updated Line").color(ChatColor.AQUA));
        sender.sendMessage(ChatComponent.text("Updated sidebar").color(ChatColor.GREEN));
    }

    @Subcommand
    public void hide(CommandSender sender) {
        Optional<Player> player = Axiom.players().stream()
                .map(p -> (Player) p)
                .findFirst();
        if (player.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No players online").color(ChatColor.RED));
            return;
        }

        Player p = player.get();
        Sidebar sidebar = sidebars.remove(p.name());
        if (sidebar != null) {
            sidebar.hide(p);
        }
        sender.sendMessage(ChatComponent.text("Hid sidebar").color(ChatColor.GREEN));
    }

}
