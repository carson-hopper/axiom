package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.Sound;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.fabric.Axiom;

@Command(name = "testplayer")
@Description("Test player title/actionbar/sound APIs")
@Permission("axiom.test")
public class TestPlayerCommand {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /testplayer <title|actionbar|sound>").color(ChatColor.RED));
    }

    @Subcommand
    public void title(CommandSender sender) {
        sender.asPlayer().ifPresent(player -> {
            ChatComponent title = ChatComponent.text("Test Title").color(ChatColor.GOLD);
            ChatComponent subtitle = ChatComponent.text("Subtitle Here").color(ChatColor.YELLOW);

            player.showTitle(title, subtitle, 5, 20, 5, 60);
            player.sendMessage(ChatComponent.text("Showed title to " + player.name()).color(ChatColor.GREEN));
        });
    }

    @Subcommand
    public void actionbar(CommandSender sender) {
        sender.asPlayer().ifPresent(player -> {
            player.sendActionBar(ChatComponent.text("Action Bar Test").color(ChatColor.AQUA));
            player.sendMessage(ChatComponent.text("Sent actionbar to " + player.name()).color(ChatColor.GREEN));
        });
    }

    @Subcommand
    public void sound(CommandSender sender) {
        sender.asPlayer().ifPresent(player -> {
            player.playSound(Sound.LEVEL_UP, 1.0f, 1.0f);

            // player.world().playSound(Sound.LEVEL_UP, 1.0f, 1.0f, player.location().position());

            player.sendMessage(ChatComponent.text("Played sound for " + player.name()).color(ChatColor.GREEN));
        });
    }

}
