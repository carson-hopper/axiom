package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Usage;
import com.axiommc.api.player.GameMode;
import com.axiommc.api.player.Player;

@Command(name = "gamemode", aliases = {"gm"})
@Description("Change a player's game mode")
@Permission("axiom.gamemode")
public class GameModeCommand {

    @Execute(type = SenderType.PLAYER)
    @Usage("<mode>")
    public void setOwnGameMode(Player sender, @Arg("mode") String mode) {
        GameMode gameMode = parseGameMode(mode);
        if (gameMode == null) {
            sender.sendMessage(ChatComponent.text(
                    "Unknown game mode: " + mode + ". Use survival, creative, adventure, or spectator.")
                    .color(ChatColor.RED));
            return;
        }
        sender.gameMode(gameMode);
        sender.sendMessage(ChatComponent.text(
                "Game mode set to " + gameMode.name().toLowerCase())
                .color(ChatColor.GREEN));
    }

    @Execute
    @Permission("axiom.gamemode.other")
    @Usage("<mode> <player>")
    public void setOtherGameMode(CommandSender sender, @Arg("mode") String mode, @Arg("target") Player target) {
        GameMode gameMode = parseGameMode(mode);
        if (gameMode == null) {
            sender.sendMessage(ChatComponent.text(
                    "Unknown game mode: " + mode + ". Use survival, creative, adventure, or spectator.")
                    .color(ChatColor.RED));
            return;
        }
        target.gameMode(gameMode);
        sender.sendMessage(ChatComponent.text(
                "Set " + target.name() + "'s game mode to " + gameMode.name().toLowerCase())
                .color(ChatColor.GREEN));
    }

    private GameMode parseGameMode(String input) {
        return switch (input.toLowerCase()) {
            case "survival", "s", "0" -> GameMode.SURVIVAL;
            case "creative", "c", "1" -> GameMode.CREATIVE;
            case "adventure", "a", "2" -> GameMode.ADVENTURE;
            case "spectator", "sp", "3" -> GameMode.SPECTATOR;
            default -> null;
        };
    }
}
