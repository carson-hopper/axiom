package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.math.Vector3;
import com.axiommc.fabric.Axiom;
import java.util.ArrayList;
import java.util.List;

@CommandMeta(
        name = "teleport",
        description = "Teleport to a player or location",
        permission = "axiom.teleport",
        aliases = "tp"
)
public class TeleportCommand implements Command {

    private static final int MIN_Y = -64;

    @Execute
    public void teleportToPlayer(CommandSender sender, @Arg("player") @DynamicTabComplete("suggestPlayers") Player target) {
        sender.asPlayer().ifPresentOrElse(
                player -> {
                    player.teleport(target.location());
                    player.sendMessage(ChatComponent.text("Teleported to " + target.name()).color(ChatColor.GREEN));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can teleport").color(ChatColor.RED))
        );
    }

    @Execute
    public void teleportPlayerToPlayer(CommandSender sender, @Arg("player1") @DynamicTabComplete("suggestPlayers") Player player1, @Arg("player2") @DynamicTabComplete("suggestPlayers") Player player2) {
        player1.teleport(player2.location());
        sender.sendMessage(ChatComponent.text("Teleported " + player1.name() + " to " + player2.name()).color(ChatColor.GREEN));
    }

    @Execute
    public void teleportToCoordinates(CommandSender sender, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        if (y < MIN_Y) {
            sender.sendMessage(ChatComponent.text("Y coordinate cannot be below " + MIN_Y).color(ChatColor.RED));
            return;
        }

        sender.asPlayer().ifPresentOrElse(
                player -> {
                    Location currentLocation = player.location();
                    Location newLocation = new Location(currentLocation.world(), new Vector3(x, y, z), currentLocation.rotation());
                    player.teleport(newLocation);
                    player.sendMessage(ChatComponent.text("Teleported to " + x + ", " + y + ", " + z).color(ChatColor.GREEN));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can teleport").color(ChatColor.RED))
        );
    }

    @Execute
    public void teleportPlayerToCoordinates(CommandSender sender, @Arg("player") @DynamicTabComplete("suggestPlayers") Player player, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        if (y < MIN_Y) {
            sender.sendMessage(ChatComponent.text("Y coordinate cannot be below " + MIN_Y).color(ChatColor.RED));
            return;
        }

        Location currentLocation = player.location();
        Location newLocation = new Location(currentLocation.world(), new Vector3(x, y, z), currentLocation.rotation());
        player.teleport(newLocation);
        sender.sendMessage(ChatComponent.text("Teleported " + player.name() + " to " + x + ", " + y + ", " + z).color(ChatColor.GREEN));
    }

    public List<String> suggestPlayers(String partial) {
        List<String> suggestions = new ArrayList<>();
        String lowerPartial = partial.toLowerCase();

        Axiom.players().forEach(player -> {
            if (player.name().toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(player.name());
            }
        });

        return suggestions;
    }
}
