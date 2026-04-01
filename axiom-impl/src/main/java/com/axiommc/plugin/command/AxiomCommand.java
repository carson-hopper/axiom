package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.PanelStyle;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.fabric.Axiom;

@Command(name = "axiom")
@Description("")
@Permission("axiom")
public class AxiomCommand {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /axiom help").color(ChatColor.RED));
    }

    @Execute
    public void help(CommandSender sender) {
        sender.sendMessage("axiom");
    }

    @Execute(type = SenderType.PLAYER)
    public void screen(Player sender) {
        Screen screen = Screen.builder()
                .title("Test Screen")
                .element(ScreenElement.Panel.of(0.1f, 0.1f, 0.8f, 0.8f, PanelStyle.DARK))
                .element(ScreenElement.Label.of(0.5f, 0.3f, "Hello World"))
                .element(ScreenElement.Button.of(0.3f, 0.6f, 0.4f, 0.1f, "Click Me", e -> {}))
                .build();
        Axiom.screenManager().open(sender, screen);
    }

}
