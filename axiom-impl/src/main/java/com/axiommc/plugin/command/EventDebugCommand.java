package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.command.annotation.Usage;
import com.axiommc.api.player.Player;

@Command(
    name = "eventdebug",
    aliases = {"ed"})
@Description("Toggle event debug logging")
@Permission("axiom.eventdebug")
public class EventDebugCommand {

    @Execute(type = SenderType.PLAYER)
    @Usage("[event]")
    public void execute(
        Player player,
        @Arg("event") String eventName) {
        boolean enabled = EventDebugListener.toggle(player.id(), eventName);
        if (enabled) {
            player.sendMessage(
                ChatComponent.text("Listening to " + eventName).color(ChatColor.GREEN));
        } else {
            player.sendMessage(
                ChatComponent.text("Stopped listening to " + eventName).color(ChatColor.RED));
        }
    }

    @Subcommand("on")
    @Execute(type = SenderType.PLAYER)
    public void on(Player player) {
        EventDebugListener.enableAll(player.id());
        player.sendMessage(
            ChatComponent.text("Event debug enabled (all events).").color(ChatColor.GREEN));
    }

    @Subcommand("off")
    @Execute(type = SenderType.PLAYER)
    public void off(Player player) {
        EventDebugListener.disableAll(player.id());
        player.sendMessage(ChatComponent.text("Event debug disabled.").color(ChatColor.RED));
    }
}
