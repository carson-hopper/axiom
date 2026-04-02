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
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.math.Vector3;

@Command(name = "teleport")
@Description("Teleport to a player or location")
public class TeleportCommand {

    private static final int MIN_Y = -64;

    @Execute(type = SenderType.PLAYER)
    @Permission("axiom.teleport")
    @Usage("<player>")
    public void teleportToPlayer(Player sender,
                                 @Arg("source") Player target) {
        sender.teleport(target.location());
        sender.sendMessage(ChatComponent.text("Teleported to " + target.name()).color(ChatColor.GREEN));
    }

    @Execute
    @Permission("axiom.teleport.other")
    @Usage("<source> <target>")
    public void teleportPlayerToPlayer(CommandSender sender, @Arg("source") Player player1, @Arg("target") Player player2) {
        player1.teleport(player2.location());
        sender.sendMessage(ChatComponent.text("Teleported " + player1.name() + " to " + player2.name()).color(ChatColor.GREEN));
    }

    @Execute(type = SenderType.PLAYER)
    @Permission("axiom.teleport")
    @Usage("<location>")
    public void teleportToCoordinates(Player sender, @Arg("location") Vector3 location) {
        if (location.y() < MIN_Y) {
            sender.sendMessage(ChatComponent.text("Y coordinate cannot be below " + MIN_Y).color(ChatColor.RED));
            return;
        }

        Location currentLocation = sender.location();
        Location newLocation = new Location(currentLocation.world(), location, currentLocation.rotation());
        sender.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported to " + location.x() + ", " + location.y() + ", " + location.z()).color(ChatColor.GREEN));
    }

    @Execute
    @Permission("axiom.teleport.other")
    @Usage("<source> <location>")
    public void teleportPlayerToCoordinates(CommandSender sender, @Arg("source") Player player, @Arg("location") Vector3 location) {
        if (location.y() < MIN_Y) {
            sender.sendMessage(ChatComponent.text("Y coordinate cannot be below " + MIN_Y).color(ChatColor.RED));
            return;
        }

        Location currentLocation = player.location();
        Location newLocation = new Location(currentLocation.world(), location, currentLocation.rotation());
        player.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported " + player.name() + " to " + location.x() + ", " + location.y() + ", " + location.z()).color(ChatColor.GREEN));
    }
}
