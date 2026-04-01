package com.axiommc.plugin.command.player;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.PanelStyle;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.fabric.Axiom;

@Command(name = "gamemode")
@Description("")
@Permission("axiom.gamemode")
public class GamemodeCommand {

    @Execute(type = SenderType.PLAYER)
    public void execute(Player sender) {

    }

}
