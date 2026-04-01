package com.axiommc.plugin.command;

import com.axiommc.api.block.Material;
import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Optional;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Usage;
import com.axiommc.api.config.PluginConfig;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.plugin.PluginContext;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collection;
import java.util.Random;

@Command(name = "rtp")
@Description("Randomly teleport to a safe location")
public class RandomTeleportCommand {

    private static final Logger LOGGER = LoggerFactory.getLogger(RandomTeleportCommand.class);
    private static final int DEFAULT_RADIUS = 500;
    private static final int MAX_ATTEMPTS = 100;
    private final PluginContext context;
    private final Random random = new Random();

    public RandomTeleportCommand(PluginContext context) {
        this.context = context;
    }

    @Execute
    @Permission("axiom.rtp")
    @Usage("[world]")
    public void execute(CommandSender sender, @Arg("world") @Optional String worldName) {
        sender.asPlayer().ifPresentOrElse(
                player -> teleportPlayer(player, worldName),
                () -> sender.sendMessage(ChatComponent.text("Only players can use /rtp").color(ChatColor.RED))
        );
    }

    private void teleportPlayer(Player player, String worldName) {
        World world = getTargetWorld(worldName, player);
        if (world == null) {
            player.sendMessage(ChatComponent.text("World not found").color(ChatColor.RED));
            return;
        }

        int radius = getRadiusForWorld(world.name());

        Location safeLocation = findSafeLocation(world, radius);
        if (safeLocation == null) {
            player.sendMessage(ChatComponent.text("Could not find a safe location").color(ChatColor.RED));
            return;
        }

        player.teleport(safeLocation);
        player.sendMessage(ChatComponent.text("Teleported to random location in " + world.name())
                .color(ChatColor.GREEN));
    }

    private World getTargetWorld(String worldName, Player player) {
        if (worldName == null || worldName.isEmpty()) {
            return player.location().world();
        }

        Collection<? extends World> worlds = Axiom.worlds();
        for (World world : worlds) {
            if (world.name().equalsIgnoreCase(worldName)) {
                return world;
            }
        }
        return null;
    }

    private int getRadiusForWorld(String worldName) {
        PluginConfig config = context.config();
        String key = "rtp." + worldName.replace("minecraft:", "") + ".radius";
        return config.getInt(key, DEFAULT_RADIUS);
    }

    private Location findSafeLocation(World world, int radius) {
        for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
            int x = random.nextInt(radius * 2 + 1) - radius;
            int z = random.nextInt(radius * 2 + 1) - radius;

            int y = world.highestBlockY(x, z) + 1;
            if (y < world.minHeight() || y > world.maxHeight()) {
                continue;
            }

            if (isSafeLocation(world, x, y, z)) {
                Vector3 position = new Vector3(x + 0.5, y, z + 0.5);
                Vector2 rotation = new Vector2(0, 0);
                return new Location(world, position, rotation);
            }
        }
        return null;
    }

    private boolean isSafeLocation(World world, int x, int y, int z) {
        try {
            Material playerBlockType = world.blockAt(x, y, z).type();
            Material headBlockType = world.blockAt(x, y + 1, z).type();
            Material groundBlockType = world.blockAt(x, y - 1, z).type();

            boolean playerBlockAir = playerBlockType == Material.AIR || playerBlockType == Material.CAVE_AIR;
            boolean headBlockAir = headBlockType == Material.AIR || headBlockType == Material.CAVE_AIR;
            boolean groundSolid = groundBlockType != Material.AIR &&
                    groundBlockType != Material.CAVE_AIR &&
                    groundBlockType != Material.VOID_AIR &&
                    groundBlockType != Material.WATER &&
                    groundBlockType != Material.LAVA;

            if (!playerBlockAir || !headBlockAir || !groundSolid) {
                LOGGER.debug("Not safe at {},{},{}: player={} head={} ground={}", x, y, z, playerBlockType, headBlockType, groundBlockType);
            }

            return playerBlockAir && headBlockAir && groundSolid;
        } catch (Exception e) {
            LOGGER.warn("Error checking safe location at {},{},{}", x, y, z, e);
            return false;
        }
    }
}
