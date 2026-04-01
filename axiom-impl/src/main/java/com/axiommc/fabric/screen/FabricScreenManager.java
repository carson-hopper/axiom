package com.axiommc.fabric.screen;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.screen.ScreenClickEvent;
import com.axiommc.api.event.screen.ScreenCloseEvent;
import com.axiommc.api.event.screen.ScreenOpenEvent;
import com.axiommc.api.gui.ClickType;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.level.GameType;
import net.minecraft.world.phys.Vec3;

import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manages virtual screen sessions backed by packet-spawned display entities.
 *
 * <p>Call {@link #tick()} every server tick, {@link #onInteract(ServerPlayer, int, boolean)}
 * from interact packet handling, and {@link #onDisconnect(ServerPlayer)} on player leave.
 */
public class FabricScreenManager implements ScreenManager {

    /** Degrees of yaw movement that maps to full screen width. */
    private static final float YAW_RANGE = 60f;
    /** Degrees of pitch movement that maps to full screen height. */
    private static final float PITCH_RANGE = 40f;

    private final EventBus eventBus;
    private final Map<UUID, ScreenSession> sessions = new ConcurrentHashMap<>();
    private final Map<UUID, UUID> playerSessions = new ConcurrentHashMap<>();
    private final Map<UUID, float[]> lastCursorPos = new ConcurrentHashMap<>();

    public FabricScreenManager(EventBus eventBus) {
        this.eventBus = eventBus;
    }

    // ── ScreenManager ────────────────────────────────────────────────────────

    @Override
    public UUID open(Player player, Screen screen) {
        FabricPlayer fabricPlayer = (FabricPlayer) player;
        ServerPlayer serverPlayer = fabricPlayer.player();

        // Close existing session if any
        UUID existing = playerSessions.get(serverPlayer.getUUID());
        if (existing != null) {
            close(existing);
        }

        // Store base yaw/pitch before locking (this is where the screen is centered)
        float baseYaw = serverPlayer.getYRot();
        float basePitch = serverPlayer.getXRot();

        // Lock camera and movement
        GameType originalMode = ScreenCameraLock.lock(serverPlayer);

        // Spawn screen entities
        ScreenEntitySpawner.SpawnResult result =
                ScreenEntitySpawner.spawnScreen(serverPlayer, screen);

        UUID sessionId = UUID.randomUUID();
        ScreenSession session = new ScreenSession(
                sessionId, serverPlayer, screen,
                result.entityIds(), result.interactionMap(), result.cursorEntityId(),
                originalMode, baseYaw, basePitch
        );
        sessions.put(sessionId, session);
        playerSessions.put(serverPlayer.getUUID(), sessionId);

        eventBus.publish(new ScreenOpenEvent(player, sessionId, screen));
        return sessionId;
    }

    @Override
    public void update(UUID sessionId, Screen screen) {
        ScreenSession session = sessions.get(sessionId);
        if (session == null) {
            return;
        }

        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());
        ScreenEntitySpawner.SpawnResult result =
                ScreenEntitySpawner.spawnScreen(session.player(), screen);

        sessions.put(sessionId, new ScreenSession(
                sessionId, session.player(), screen,
                result.entityIds(), result.interactionMap(), result.cursorEntityId(),
                session.originalMode(), session.baseYaw(), session.basePitch()
        ));
    }

    @Override
    public void close(UUID sessionId) {
        ScreenSession session = sessions.remove(sessionId);
        if (session == null) {
            return;
        }

        playerSessions.remove(session.player().getUUID());
        lastCursorPos.remove(sessionId);
        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());
        ScreenCameraLock.unlock(session.player(), session.originalMode());
        session.screen().handleClose(new FabricPlayer(session.player()));

        eventBus.publish(new ScreenCloseEvent(new FabricPlayer(session.player()), sessionId));
    }

    @Override
    public boolean isOpen(UUID sessionId) {
        return sessions.containsKey(sessionId);
    }

    // ── Tick (cursor tracking) ───────────────────────────────────────────────

    /**
     * Must be called every server tick. Updates cursor position for each session
     * by converting the player's head rotation delta into screen coordinates.
     */
    public void tick() {
        for (ScreenSession session : sessions.values()) {
            ServerPlayer player = session.player();
            if (!player.isAlive()) {
                close(session.sessionId());
                continue;
            }
            updateCursor(session);
        }
    }

    private void updateCursor(ScreenSession session) {
        ServerPlayer player = session.player();
        Screen screen = session.screen();

        // Convert yaw/pitch delta from base position to normalized screen coords
        float yawDelta = player.getYRot() - session.baseYaw();
        float pitchDelta = player.getXRot() - session.basePitch();

        // Normalize to 0..1 range (center = 0.5, 0.5)
        float u = 0.5f + (yawDelta / YAW_RANGE);
        float v = 0.5f + (pitchDelta / PITCH_RANGE);
        u = Math.max(0f, Math.min(1f, u));
        v = Math.max(0f, Math.min(1f, v));

        // Skip update if cursor hasn't moved significantly
        float[] last = lastCursorPos.get(session.sessionId());
        if (last != null && Math.abs(u - last[0]) < 0.001f && Math.abs(v - last[1]) < 0.001f) {
            return;
        }
        lastCursorPos.put(session.sessionId(), new float[]{u, v});

        // Calculate world position for cursor entity
        Vec3 eyePos = player.getEyePosition();
        float baseYawRad = (float) Math.toRadians(session.baseYaw());
        float basePitchRad = (float) Math.toRadians(session.basePitch());

        // Forward direction based on BASE yaw/pitch (not current look)
        Vec3 forward = new Vec3(
                -Math.sin(baseYawRad) * Math.cos(basePitchRad),
                -Math.sin(basePitchRad),
                Math.cos(baseYawRad) * Math.cos(basePitchRad)
        ).normalize();
        Vec3 right = forward.cross(new Vec3(0, 1, 0)).normalize();
        Vec3 up = right.cross(forward).normalize();
        Vec3 center = eyePos.add(forward.scale(screen.distance()));

        // Map (u, v) to world position on screen plane
        double offsetX = (u - 0.5) * screen.width();
        double offsetY = (0.5 - v) * screen.height();
        Vec3 cursorPos = center
                .add(right.scale(offsetX))
                .add(up.scale(offsetY))
                .add(forward.scale(-0.3)); // in front of panel

        ScreenEntitySpawner.moveCursor(player, session.cursorEntityId(), cursorPos);
    }

    // ── Interact routing ────────────────────────────────────────────────────

    /**
     * Route a client interact packet to the correct ScreenClickHandler.
     *
     * @param player     the player who interacted
     * @param entityId   the entity ID that was interacted with
     * @param isMainHand true for main hand / left-click
     */
    public void onInteract(ServerPlayer player, int entityId, boolean isMainHand) {
        UUID sessionId = playerSessions.get(player.getUUID());
        if (sessionId == null) {
            return;
        }

        ScreenSession session = sessions.get(sessionId);
        if (session == null) {
            return;
        }

        ScreenElement element = session.interactionMap().get(entityId);
        if (element == null) {
            return;
        }

        ClickType clickType = isMainHand ? ClickType.LEFT : ClickType.RIGHT;
        Vector2 cursor = computeCursorNorm(player, session);

        ScreenClickEvent event = new ScreenClickEvent(
                new FabricPlayer(player), cursor, clickType);

        switch (element) {
            case ScreenElement.Button button -> button.onClick().onClick(event);
            case ScreenElement.ItemSlot slot -> {
                if (slot.onClick() != null) {
                    slot.onClick().onClick(event);
                }
            }
            default -> {}
        }
    }

    /**
     * Called when the player presses sneak to close the screen.
     */
    public void onSneak(ServerPlayer player) {
        UUID sessionId = playerSessions.get(player.getUUID());
        if (sessionId != null) {
            close(sessionId);
        }
    }

    // ── Disconnect cleanup ──────────────────────────────────────────────────

    /**
     * Clean up a player's session on disconnect without sending packets.
     */
    public void onDisconnect(ServerPlayer player) {
        UUID sessionId = playerSessions.remove(player.getUUID());
        if (sessionId == null) {
            return;
        }
        ScreenSession session = sessions.remove(sessionId);
        if (session == null) {
            return;
        }
        // Don't send packets — player is gone
        // Restore game mode for next login
        ScreenCameraLock.unlock(player, session.originalMode());
        session.screen().handleClose(new FabricPlayer(player));
        eventBus.publish(new ScreenCloseEvent(new FabricPlayer(player), sessionId));
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    private Vector2 computeCursorNorm(ServerPlayer player, ScreenSession session) {
        float yawDelta = player.getYRot() - session.baseYaw();
        float pitchDelta = player.getXRot() - session.basePitch();
        float u = Math.max(0f, Math.min(1f, 0.5f + yawDelta / YAW_RANGE));
        float v = Math.max(0f, Math.min(1f, 0.5f + pitchDelta / PITCH_RANGE));
        return new Vector2(u, v);
    }

    /**
     * Returns whether the given player has an open screen session.
     */
    public boolean hasSession(ServerPlayer player) {
        return playerSessions.containsKey(player.getUUID());
    }
}
