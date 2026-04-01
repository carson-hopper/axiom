package com.axiommc.fabric.screen;

import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import net.minecraft.server.level.ServerPlayer;

import java.util.List;
import java.util.Map;
import java.util.UUID;

/**
 * Tracks one open screen session for a player.
 *
 * @param sessionId      the unique session identifier
 * @param player         the player viewing the screen
 * @param screen         the screen definition
 * @param entityIds      IDs of all packet-spawned display entities (for despawning)
 * @param interactionMap maps interaction entity ID → the ScreenElement it represents
 * @param cursorEntityId entity ID of the cursor dot text_display
 */
public record ScreenSession(
        UUID sessionId,
        ServerPlayer player,
        Screen screen,
        List<Integer> entityIds,
        Map<Integer, ScreenElement> interactionMap,
        int cursorEntityId
) {}
