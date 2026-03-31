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
import com.axiommc.fabric.Axiom;
import com.axiommc.api.math.Vector3;

@CommandMeta(
        name = "teleport",
        description = "Teleport to a player or location",
        permission = "axiom.teleport",
        aliases = "tp"
)
public class TeleportCommand implements Command {

    @Execute
    public void teleportToPlayer(CommandSender sender, @Arg("player") String playerName) {
        if (!sender.isPlayer()) {
            sender.sendMessage(ChatComponent.text("Only players can teleport").color(ChatColor.RED));
            return;
        }

        Player senderPlayer = sender.asPlayer().get();
        var targetPlayer = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        if (targetPlayer.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Player not found: " + playerName).color(ChatColor.RED));
            return;
        }

        Player target = targetPlayer.get();
        senderPlayer.teleport(target.location());
        sender.sendMessage(ChatComponent.text("Teleported to " + target.name()).color(ChatColor.GREEN));
    }

    @Execute
    public void teleportPlayerToPlayer(CommandSender sender, @Arg("player1") String player1Name, @Arg("player2") String player2Name) {
        var player1 = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(player1Name))
                .findFirst();

        var player2 = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(player2Name))
                .findFirst();

        if (player1.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Player not found: " + player1Name).color(ChatColor.RED));
            return;
        }

        if (player2.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Player not found: " + player2Name).color(ChatColor.RED));
            return;
        }

        Player p1 = player1.get();
        Player p2 = player2.get();
        p1.teleport(p2.location());
        sender.sendMessage(ChatComponent.text("Teleported " + p1.name() + " to " + p2.name()).color(ChatColor.GREEN));
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
    public void teleportPlayerToCoordinates(CommandSender sender, @Arg("player") String playerName, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        var targetPlayer = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        if (targetPlayer.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Player not found: " + playerName).color(ChatColor.RED));
            return;
        }

        Player player = targetPlayer.get();
        Location currentLocation = player.location();
        Location newLocation = new Location(currentLocation.world(), new Vector3(x, y, z), currentLocation.rotation());
        player.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported " + player.name() + " to " + x + ", " + y + ", " + z).color(ChatColor.GREEN));
    }
}
