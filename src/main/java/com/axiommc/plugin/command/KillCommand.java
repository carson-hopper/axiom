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
import com.axiommc.fabric.player.FabricPlayer;

@CommandMeta(
        name = "kill",
        description = "Kill a player",
        permission = "axiom.kill"
)
public class KillCommand implements Command {

    @Execute
    public void execute(CommandSender sender) {
        sender.asPlayer().ifPresentOrElse(
                player -> {
                    player.damage(player.health());
                    player.sendMessage(ChatComponent.text("You have been killed").color(ChatColor.RED));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can be killed").color(ChatColor.RED))
        );
    }

    @Subcommand
    public void player(CommandSender sender, @Arg("player") Player player) {
        player.damage(player.health());
        sender.sendMessage(ChatComponent.text("Killed " + player.name()).color(ChatColor.RED));
    }
}
