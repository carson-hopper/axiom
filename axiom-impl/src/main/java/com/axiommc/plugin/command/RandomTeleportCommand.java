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
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

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
    public void execute(CommandSender sender) {
        sender.asPlayer().ifPresentOrElse(
                player -> {
                    int radius = getRadiusForWorld(player.world().name());
                    Location safeLocation = findSafeLocation(player.world(), radius);
                    if (safeLocation == null) {
                        player.sendMessage(ChatComponent.text("Could not find a safe location").color(ChatColor.RED));
                        return;
                    }

                    player.teleport(safeLocation);
                    player.sendMessage(ChatComponent.text("Teleported to random location").color(ChatColor.GREEN));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can use /rtp").color(ChatColor.RED))
        );
    }

    @Execute
    @Permission("axiom.rtp.world")
    @Usage("[world]")
    public void execute(CommandSender sender, @Arg("world") @Optional World world) {
        sender.asPlayer().ifPresentOrElse(
                player -> {
                    World targetWorld = world != null ? world : player.world();

                    int radius = getRadiusForWorld(targetWorld.name());
                    Location safeLocation = findSafeLocation(targetWorld, radius);
                    if (safeLocation == null) {
                        player.sendMessage(ChatComponent.text("Could not find a safe location").color(ChatColor.RED));
                        return;
                    }

                    player.teleport(safeLocation);
                    player.sendMessage(ChatComponent.text("Teleported to random location in " + targetWorld.name())
                            .color(ChatColor.GREEN));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can use /rtp").color(ChatColor.RED))
        );
    }

    @Execute
    @Permission("axiom.rtp.other")
    @Usage("<target> [world]")
    public void execute(CommandSender sender, @Arg("target") @Optional Player player, @Arg("world") @Optional World world) {
        World targetWorld = world != null ? world : player.world();

        int radius = getRadiusForWorld(targetWorld.name());
        Location safeLocation = findSafeLocation(targetWorld, radius);
        if (safeLocation == null) {
            sender.sendMessage(ChatComponent.text("Could not find a safe location for " + player.name()).color(ChatColor.RED));
            return;
        }

        player.teleport(safeLocation);
        player.sendMessage(ChatComponent.text("Teleported to random location in " + targetWorld.name()).color(ChatColor.GREEN));
        sender.sendMessage(ChatComponent.text("Teleported " + player.name() + " to random location in " + targetWorld.name()).color(ChatColor.GREEN));
    }

    private int getRadiusForWorld(String worldName) {
        PluginConfig config = context.config();
        String key = "rtp." + worldName.replace("minecraft:", "") + ".radius";
        return context.config().getInt(key, DEFAULT_RADIUS);
    }

    private Location findSafeLocation(World world, int radius) {
        int seaLevel = 64;

        for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
            int x = random.nextInt(radius * 2 + 1) - radius;
            int z = random.nextInt(radius * 2 + 1) - radius;

            // Search from sea level upwards
            for (int y = seaLevel; y < world.maxHeight(); y++) {
                if (isSafeLocation(world, x, y, z)) {
                    Vector3 position = new Vector3(x + 0.5, y, z + 0.5);
                    Vector2 rotation = new Vector2(0, 0);
                    return new Location(world, position, rotation);
                }
            }

            // If not found going up, try going down from sea level
            for (int y = seaLevel - 1; y >= world.minHeight(); y--) {
                if (isSafeLocation(world, x, y, z)) {
                    Vector3 position = new Vector3(x + 0.5, y, z + 0.5);
                    Vector2 rotation = new Vector2(0, 0);
                    return new Location(world, position, rotation);
                }
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
