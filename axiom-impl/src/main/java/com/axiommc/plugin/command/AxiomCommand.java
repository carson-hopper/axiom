package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;

import com.axiommc.api.chat.ChatDecoration;
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
                .width(3.5f)
                .height(2.5f)
                .distance(2.0f)          // slightly closer
                // Dark outer background
                .element(ScreenElement.Panel.of(0f, 0f, 1f, 1f, PanelStyle.DARK))
                // Glass inner card
                .element(ScreenElement.Panel.of(0.05f, 0.05f, 0.9f, 0.9f, PanelStyle.GLASS))
                // Title label
                .element(ScreenElement.Label.of(0.5f, 0.15f,
                        ChatComponent.text("Test Screen", ChatColor.AQUA)
                                .decoration(ChatDecoration.BOLD, true)))
                // Divider label
                .element(ScreenElement.Label.of(0.5f, 0.28f,
                        ChatComponent.text("─────────────", ChatColor.DARK_GRAY)))
                // Button with accent border panel behind it
                .element(ScreenElement.Panel.of(0.25f, 0.58f, 0.5f, 0.15f, PanelStyle.ACCENT))
                .element(ScreenElement.Button.of(0.25f, 0.58f, 0.5f, 0.15f,
                        ChatComponent.text("Click Me", ChatColor.WHITE),
                        e -> {}))
                .build();

        Axiom.screenManager().open(sender, screen);
    }

}
