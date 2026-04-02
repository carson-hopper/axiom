package com.axiommc.plugin.command.player;

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
    public void setOwnGameMode(Player sender, @Arg("mode") GameMode gameMode) {
        sender.gameMode(gameMode);
        sender.sendMessage(ChatComponent.textf("Game mode set to {}", gameMode.name().toLowerCase())
                .color(ChatColor.GREEN));
    }

    @Execute
    @Permission("axiom.gamemode.other")
    @Usage("<mode> <player>")
    public void setOtherGameMode(CommandSender sender,
                                 @Arg("mode") GameMode gameMode,
                                 @Arg("target") Player target) {
        target.gameMode(gameMode);
        sender.sendMessage(ChatComponent.textf("Set {}'s game mode to {}", target.name(), gameMode.name().toLowerCase())
                .color(ChatColor.GREEN));
    }
}
