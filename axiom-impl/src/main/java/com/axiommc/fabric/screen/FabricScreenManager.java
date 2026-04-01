package com.axiommc.fabric.screen;

import com.axiommc.api.player.Player;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.server.level.ServerPlayer;

import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manages virtual screen sessions backed by packet-spawned display entities.
 */
public class FabricScreenManager implements ScreenManager {

    private final Map<UUID, ScreenSession> sessions = new ConcurrentHashMap<>();
    private final Map<UUID, UUID> playerSessions = new ConcurrentHashMap<>();

    @Override
    public UUID open(Player player, Screen screen) {
        FabricPlayer fabricPlayer = (FabricPlayer) player;
        ServerPlayer serverPlayer = fabricPlayer.player();

        // Close existing screen if any
        UUID existingSession = playerSessions.get(serverPlayer.getUUID());
        if (existingSession != null) {
            close(existingSession);
        }

        UUID sessionId = UUID.randomUUID();
        List<Integer> entityIds = ScreenEntitySpawner.spawnScreen(serverPlayer, screen);

        ScreenSession session = new ScreenSession(sessionId, serverPlayer, screen, entityIds);
        sessions.put(sessionId, session);
        playerSessions.put(serverPlayer.getUUID(), sessionId);

        Axiom.logger().debug("Opened screen session {} for {}", sessionId, player.name());
        return sessionId;
    }

    @Override
    public void update(UUID sessionId, Screen screen) {
        ScreenSession session = sessions.get(sessionId);
        if (session == null) {
            return;
        }

        // Despawn old entities
        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());

        // Spawn new entities
        List<Integer> newEntityIds = ScreenEntitySpawner.spawnScreen(session.player(), screen);

        // Replace session
        sessions.put(sessionId, new ScreenSession(sessionId, session.player(), screen, newEntityIds));
    }

    @Override
    public void close(UUID sessionId) {
        ScreenSession session = sessions.remove(sessionId);
        if (session == null) {
            return;
        }

        playerSessions.remove(session.player().getUUID());
        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());
        session.screen().handleClose(new FabricPlayer(session.player()));

        Axiom.logger().debug("Closed screen session {}", sessionId);
    }

    @Override
    public boolean isOpen(UUID sessionId) {
        return sessions.containsKey(sessionId);
    }
}
