package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerCommandEvent;
import com.axiommc.api.event.player.PlayerInteractEntityEvent;
import com.axiommc.api.event.player.PlayerPositionEvent;
import com.axiommc.api.event.player.PlayerSwapHandItemsEvent;
import com.axiommc.api.event.player.PlayerToggleEvent;
import com.axiommc.api.math.Vector3;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires player input events (movement, commands, entity interaction,
 * sprint/sneak toggles, and hand-swap). Called from
 * {@code ServerGamePacketListenerMixin}.
 */
public class PlayerInputAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerInputAdapter.eventBus = eventBus;
        PlayerInputAdapter.playerProvider = playerProvider;
    }

    /**
     * Fires a {@link PlayerPositionEvent.Move}.
     *
     * @return true if the event was cancelled
     */
    public static boolean onMove(ServerPlayer serverPlayer,
                                 double fromX, double fromY, double fromZ,
                                 double toX, double toY, double toZ) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Vector3 from = new Vector3(fromX, fromY, fromZ);
            Vector3 to = new Vector3(toX, toY, toZ);
            PlayerPositionEvent.Move event = new PlayerPositionEvent.Move(player, from, to);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerPositionEvent.Move", exception);
            return false;
        }
    }

    /**
     * Fires a {@link PlayerCommandEvent}.
     *
     * @return true if the event was cancelled
     */
    public static boolean onCommand(ServerPlayer serverPlayer, String command) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerCommandEvent event = new PlayerCommandEvent(player, command);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerCommandEvent", exception);
            return false;
        }
    }

    /**
     * Fires a {@link PlayerInteractEntityEvent}.
     *
     * @return true if the event was cancelled
     */
    public static boolean onInteractEntity(ServerPlayer serverPlayer, int entityId, boolean mainHand) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerInteractEntityEvent event = new PlayerInteractEntityEvent(
                    player, entityId, mainHand);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerInteractEntityEvent", exception);
            return false;
        }
    }

    /**
     * Fires a {@link PlayerToggleEvent.Sprint}.
     */
    public static void onToggleSprint(ServerPlayer serverPlayer, boolean sprinting) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerToggleEvent.Sprint(player, sprinting));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerToggleEvent.Sprint", exception);
        }
    }

    /**
     * Fires a {@link PlayerToggleEvent.Sneak}.
     */
    public static void onToggleSneak(ServerPlayer serverPlayer, boolean sneaking) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerToggleEvent.Sneak(player, sneaking));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerToggleEvent.Sneak", exception);
        }
    }

    /**
     * Fires a {@link PlayerSwapHandItemsEvent}.
     *
     * @return true if the event was cancelled
     */
    public static boolean onSwapHandItems(ServerPlayer serverPlayer) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerSwapHandItemsEvent event = new PlayerSwapHandItemsEvent(player);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerSwapHandItemsEvent", exception);
            return false;
        }
    }
}
