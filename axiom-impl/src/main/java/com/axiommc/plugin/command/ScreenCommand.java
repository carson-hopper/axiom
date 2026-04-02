package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.chat.ChatDecoration;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.PanelStyle;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.fabric.Axiom;

@Command(
    name = "screen",
    aliases = {"sc"})
@Permission("axiom.screen")
public class ScreenCommand {

    @Execute(type = SenderType.PLAYER)
    public void onDefault(Player sender) {
        sender.sendMessage(
            ChatComponent.text("Usage: /screen test | /screen close", ChatColor.YELLOW));
    }

    @Subcommand("test")
    @Execute(type = SenderType.PLAYER)
    public void onTest(Player sender) {
        Screen screen = Screen.builder()
            .title("Test Screen")
            .width(2.5f)
            .height(1.8f)
            .distance(2.0f)
            .element(ScreenElement.Panel.of(0f, 0f, 1f, 1f, PanelStyle.DARK))
            .element(ScreenElement.Panel.of(0.04f, 0.08f, 0.92f, 0.84f, PanelStyle.GLASS))
            .element(ScreenElement.Label.of(
                0.5f,
                0.18f,
                ChatComponent.text("⬡ Axiom", ChatColor.AQUA)
                    .decoration(ChatDecoration.BOLD, true)))
            .element(ScreenElement.Label.of(
                0.5f, 0.32f, ChatComponent.text("Screen System", ChatColor.GRAY)))
            .element(ScreenElement.Button.of(
                0.25f,
                0.55f,
                0.5f,
                0.18f,
                ChatComponent.text("✕ Close", ChatColor.WHITE),
                e -> Axiom.screenManager().close(e.player())))
            .build();

        Axiom.screenManager().open(sender, screen);
        sender.sendMessage(ChatComponent.text("Screen opened.", ChatColor.GREEN));
    }

    @Subcommand("close")
    @Execute(type = SenderType.PLAYER)
    public void onClose(Player sender) {
        ScreenManager manager = Axiom.screenManager();
        if (!manager.isOpen(sender)) {
            sender.sendMessage(ChatComponent.text("No screen is open.", ChatColor.RED));
            return;
        }
        manager.close(sender);
        sender.sendMessage(ChatComponent.text("Screen closed.", ChatColor.GREEN));
    }
}
