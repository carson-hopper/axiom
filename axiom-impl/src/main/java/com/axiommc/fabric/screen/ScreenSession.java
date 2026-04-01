package com.axiommc.fabric.screen;

import com.axiommc.api.screen.Screen;
import net.minecraft.server.level.ServerPlayer;

import java.util.List;
import java.util.UUID;

/**
 * Tracks one open screen session for a player.
 *
 * @param sessionId the unique session identifier
 * @param player    the player viewing the screen
 * @param screen    the screen definition
 * @param entityIds IDs of all packet-spawned display entities (for despawning)
 */
public record ScreenSession(
        UUID sessionId,
        ServerPlayer player,
        Screen screen,
        List<Integer> entityIds
) {}
