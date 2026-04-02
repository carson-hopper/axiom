package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerDeathEvent;
import com.axiommc.api.event.player.PlayerKickEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerKickEvent} and {@link PlayerDeathEvent.Respawn} from
 * {@code PlayerListMixin} injections.
 */
public class PlayerLifecycleAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerLifecycleAdapter.eventBus = eventBus;
        PlayerLifecycleAdapter.playerProvider = playerProvider;
    }

    /**
     * Called from the mixin when a player is removed from the server.
     *
     * @param serverPlayer the player being kicked
     * @param reason       the kick reason
     */
    public static void onKick(ServerPlayer serverPlayer, String reason) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerKickEvent(player, reason));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerKickEvent", exception);
        }
    }

    /**
     * Called from the mixin when a player respawns.
     *
     * @param serverPlayer the player respawning
     */
    public static void onRespawn(ServerPlayer serverPlayer) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerDeathEvent.Respawn(player));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerRespawnEvent", exception);
        }
    }
}
