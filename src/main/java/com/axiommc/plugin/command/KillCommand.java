package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;

@CommandMeta(
        name = "kill",
        description = "Kill a player",
        permission = "axiom.kill"
)
public class KillCommand implements Command {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /kill [player]").color(ChatColor.RED));
    }

    @Subcommand
    public void self(CommandSender sender) {
        if (!sender.isPlayer()) {
            sender.sendMessage(ChatComponent.text("Only players can be killed").color(ChatColor.RED));
            return;
        }

        Player player = sender.asPlayer().get();
        player.health(0);
        sender.sendMessage(ChatComponent.text("You have been killed").color(ChatColor.RED));
    }

    @Subcommand
    public void player(CommandSender sender, @Arg("player") String playerName) {
        var target = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        if (target.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Player not found: " + playerName).color(ChatColor.RED));
            return;
        }

        Player player = (Player) target.get();
        player.health(0);
        sender.sendMessage(ChatComponent.text("Killed " + playerName).color(ChatColor.RED));
    }
}
