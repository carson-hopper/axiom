package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.math.Vector3;

@CommandMeta(
        name = "teleport",
        description = "Teleport to a player or location",
        permission = "axiom.teleport",
        aliases = "tp"
)
public class TeleportCommand implements Command {

    @Execute
    public void teleportToPlayer(CommandSender sender, @Arg("player") Player target) {
        if (!sender.isPlayer()) {
            sender.sendMessage(ChatComponent.text("Only players can teleport").color(ChatColor.RED));
            return;
        }

        Player senderPlayer = sender.asPlayer().get();
        senderPlayer.teleport(target.location());
        sender.sendMessage(ChatComponent.text("Teleported to " + target.name()).color(ChatColor.GREEN));
    }

    @Execute
    public void teleportPlayerToPlayer(CommandSender sender, @Arg("player1") Player player1, @Arg("player2") Player player2) {
        player1.teleport(player2.location());
        sender.sendMessage(ChatComponent.text("Teleported " + player1.name() + " to " + player2.name()).color(ChatColor.GREEN));
    }

    @Execute
    public void teleportToCoordinates(CommandSender sender, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        if (!sender.isPlayer()) {
            sender.sendMessage(ChatComponent.text("Only players can teleport").color(ChatColor.RED));
            return;
        }

        Player senderPlayer = sender.asPlayer().get();
        Location currentLocation = senderPlayer.location();
        Location newLocation = new Location(currentLocation.world(), new Vector3(x, y, z), currentLocation.rotation());
        senderPlayer.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported to " + x + ", " + y + ", " + z).color(ChatColor.GREEN));
    }

    @Execute
    public void teleportPlayerToCoordinates(CommandSender sender, @Arg("player") Player player, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        Location currentLocation = player.location();
        Location newLocation = new Location(currentLocation.world(), new Vector3(x, y, z), currentLocation.rotation());
        player.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported " + player.name() + " to " + x + ", " + y + ", " + z).color(ChatColor.GREEN));
    }
}
