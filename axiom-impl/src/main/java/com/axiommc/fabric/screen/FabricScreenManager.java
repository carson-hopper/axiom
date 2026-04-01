package com.axiommc.fabric.screen;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.screen.ScreenCloseEvent;
import com.axiommc.api.event.screen.ScreenOpenEvent;
import com.axiommc.api.gui.ClickType;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.player.Player;
import com.axiommc.api.event.screen.ScreenClickEvent;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.phys.Vec3;

import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manages virtual screen sessions backed by packet-spawned display entities.
 *
 * <p>Register this once and call {@link #tick()} from your server tick event,
 * {@link #onInteract(ServerPlayer, int, boolean)} from your interact packet
 * mixin, and {@link #onDisconnect(ServerPlayer)} from your disconnect listener.
 */
public class FabricScreenManager implements ScreenManager {

    private final EventBus eventBus;
    private final Map<UUID, ScreenSession> sessions = new ConcurrentHashMap<>();
    private final Map<UUID, UUID> playerSessions = new ConcurrentHashMap<>();

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
        if (existing != null) close(existing);

        UUID sessionId = UUID.randomUUID();
        ScreenEntitySpawner.SpawnResult result = ScreenEntitySpawner.spawnScreen(serverPlayer, screen);

        ScreenSession session = new ScreenSession(
                sessionId, serverPlayer, screen,
                result.entityIds(), result.interactionMap(), result.cursorEntityId()
        );
        sessions.put(sessionId, session);
        playerSessions.put(serverPlayer.getUUID(), sessionId);

        eventBus.publish(new ScreenOpenEvent(player, sessionId, screen));
        Axiom.logger().debug("Opened screen session {} for {}", sessionId, player.name());
        return sessionId;
    }

    @Override
    public void update(UUID sessionId, Screen screen) {
        ScreenSession session = sessions.get(sessionId);
        if (session == null) return;

        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());

        ScreenEntitySpawner.SpawnResult result = ScreenEntitySpawner.spawnScreen(session.player(), screen);
        sessions.put(sessionId, new ScreenSession(
                sessionId, session.player(), screen,
                result.entityIds(), result.interactionMap(), result.cursorEntityId()
        ));
    }

    @Override
    public void close(UUID sessionId) {
        ScreenSession session = sessions.remove(sessionId);
        if (session == null) return;

        playerSessions.remove(session.player().getUUID());
        ScreenEntitySpawner.despawnEntities(session.player(), session.entityIds());
        session.screen().handleClose(new FabricPlayer(session.player()));

        eventBus.publish(new ScreenCloseEvent(new FabricPlayer(session.player()), sessionId));
        Axiom.logger().debug("Closed screen session {}", sessionId);
    }

    @Override
    public void close(Player player) {
        FabricPlayer fabricPlayer = (FabricPlayer) player;
        UUID sessionId = playerSessions.get(fabricPlayer.player().getUUID());
        if (sessionId != null) close(sessionId);
    }

    @Override
    public boolean isOpen(Player player) {
        FabricPlayer fabricPlayer = (FabricPlayer) player;
        return playerSessions.containsKey(fabricPlayer.player().getUUID());
    }

    @Override
    public boolean isOpen(UUID sessionId) {
        return sessions.containsKey(sessionId);
    }

    // ── Tick ────────────────────────────────────────────────────────────────

    /**
     * Must be called every server tick. Updates the cursor position for each
     * active session by projecting the player's look direction onto the screen plane.
     */
    public void tick() {
        for (ScreenSession session : sessions.values()) {
            ServerPlayer player = session.player();
            if (!player.isAlive()) {
                close(session.sessionId());
                continue;
            }
            moveCursor(session);
        }
    }

    private void moveCursor(ScreenSession session) {
        ServerPlayer player = session.player();
        Screen screen = session.screen();

        Vec3 eyePos  = player.getEyePosition();
        Vec3 forward = player.getLookAngle().normalize();
        Vec3 right   = forward.cross(new Vec3(0, 1, 0)).normalize();
        Vec3 up      = right.cross(forward).normalize();

        // Project look direction onto the screen plane to get cursor world pos
        Vec3 center = eyePos.add(forward.scale(screen.distance()));

        // Derive cursor (u, v) from look angle delta vs. screen axes
        double u = forward.dot(right) + 0.5;
        double v = 0.5 - forward.dot(up);
        u = Math.max(0, Math.min(1, u));
        v = Math.max(0, Math.min(1, v));

        double offsetX = (u - 0.5) * screen.width();
        double offsetY = (0.5 - v) * screen.height();
        Vec3 cursorPos = center
                .add(right.scale(offsetX))
                .add(up.scale(offsetY))
                .add(forward.scale(-0.1)); // slightly in front

        ScreenEntitySpawner.moveCursor(player, session.cursorEntityId(), cursorPos);
    }

    // ── Interact routing ────────────────────────────────────────────────────

    /**
     * Route a client interact packet to the correct {@link com.axiommc.api.screen.ScreenClickHandler}.
     *
     * <p>Call this from a mixin on {@code ServerGamePacketListenerImpl} or a
     * {@code ServerboundInteractPacket} listener.
     *
     * @param player       the player who interacted
     * @param entityId     the entity ID that was interacted with
     * @param isMainHand   true for left-click / main hand
     */
    public void onInteract(ServerPlayer player, int entityId, boolean isMainHand) {
        UUID sessionId = playerSessions.get(player.getUUID());
        if (sessionId == null) return;

        ScreenSession session = sessions.get(sessionId);
        if (session == null) return;

        ScreenElement element = session.interactionMap().get(entityId);
        if (element == null) return;

        ClickType clickType = isMainHand ? ClickType.LEFT : ClickType.RIGHT;

        // Compute cursor position at click time
        Vector2 cursor = computeCursorNorm(player, session.screen());

        ScreenClickEvent event = new ScreenClickEvent(new FabricPlayer(player), cursor, clickType);

        switch (element) {
            case ScreenElement.Button button   -> button.onClick().onClick(event);
            case ScreenElement.ItemSlot slot   -> { if (slot.onClick() != null) slot.onClick().onClick(event); }
            default -> {}
        }
    }

    // ── Disconnect ──────────────────────────────────────────────────────────

    /**
     * Clean up a player's session on disconnect without sending packets.
     */
    public void onDisconnect(ServerPlayer player) {
        UUID sessionId = playerSessions.remove(player.getUUID());
        if (sessionId == null) return;
        ScreenSession session = sessions.remove(sessionId);
        if (session == null) return;
        // Do not send despawn packets — player is gone
        session.screen().handleClose(new FabricPlayer(player));
        eventBus.publish(new ScreenCloseEvent(new FabricPlayer(player), sessionId));
    }

    // ── Helpers ─────────────────────────────────────────────────────────────

    private Vector2 computeCursorNorm(ServerPlayer player, Screen screen) {
        Vec3 forward = player.getLookAngle().normalize();
        Vec3 right   = forward.cross(new Vec3(0, 1, 0)).normalize();
        Vec3 up      = right.cross(forward).normalize();

        float u = (float) Math.max(0, Math.min(1, forward.dot(right) + 0.5));
        float v = (float) Math.max(0, Math.min(1, 0.5 - forward.dot(up)));
        return new Vector2(u, v);
    }
}
