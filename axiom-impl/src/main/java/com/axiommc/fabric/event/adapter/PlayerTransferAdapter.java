package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.connection.PostTransferEvent;
import com.axiommc.api.event.connection.PreTransferEvent;
import com.axiommc.api.world.Server;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PreTransferEvent} and {@link PostTransferEvent} when a player
 * is transferred to another server via {@code ClientboundTransferPacket}.
 */
public class PlayerTransferAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerTransferAdapter.eventBus = eventBus;
    }

    /**
     * Called before a transfer packet is sent.
     *
     * @param serverPlayer the player being transferred
     * @param host         the target server host
     * @param port         the target server port
     * @return true if the event was cancelled
     */
    public static boolean onPreTransfer(ServerPlayer serverPlayer, String host, int port) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Server targetServer = new Server(host, host, port);
            PreTransferEvent event = new PreTransferEvent(player, targetServer);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PreTransferEvent", exception);
        }
        return false;
    }

    /**
     * Called after a transfer packet is sent.
     *
     * @param serverPlayer the player being transferred
     * @param host         the target server host
     * @param port         the target server port
     */
    public static void onPostTransfer(ServerPlayer serverPlayer, String host, int port) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Server targetServer = new Server(host, host, port);
            eventBus.publish(new PostTransferEvent(player, targetServer));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PostTransferEvent", exception);
        }
    }
}
