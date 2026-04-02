package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerResourcePackStatusEvent;
import com.axiommc.api.event.player.ServerResourcePackEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.network.protocol.common.ServerboundResourcePackPacket;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerResourcePackStatusEvent} when the client responds
 * to a resource pack request.
 * Called from {@code ServerCommonPacketListenerMixin}.
 */
public class ResourcePackAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ResourcePackAdapter.eventBus = eventBus;
        ResourcePackAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when the client sends a resource pack status response.
     */
    public static void onResourcePackStatus(
            ServerPlayer serverPlayer,
            ServerboundResourcePackPacket.Action action) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerResourcePackStatusEvent.Status status =
                    mapAction(action);
            eventBus.publish(
                    new PlayerResourcePackStatusEvent(player, status));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerResourcePackStatusEvent",
                    exception);
        }
    }

    /**
     * Called when the server sends a resource pack push packet.
     */
    public static void onResourcePackSend(
            ServerPlayer serverPlayer, String packUrl,
            String packHash, boolean required) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(
                    new ServerResourcePackEvent.Send(player, packUrl));
            eventBus.publish(new ServerResourcePackEvent.Request(
                    player, packUrl, packHash, required));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing ServerResourcePackSendEvent",
                    exception);
        }
    }

    /**
     * Called when the server sends a resource pack pop (remove) packet.
     */
    public static void onResourcePackRemove(
            ServerPlayer serverPlayer) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(
                    new ServerResourcePackEvent.Remove(player));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing ServerResourcePackRemoveEvent",
                    exception);
        }
    }

    private static PlayerResourcePackStatusEvent.Status mapAction(
            ServerboundResourcePackPacket.Action action) {
        return switch (action) {
            case ACCEPTED -> PlayerResourcePackStatusEvent.Status.ACCEPTED;
            case DECLINED -> PlayerResourcePackStatusEvent.Status.DECLINED;
            case SUCCESSFULLY_LOADED ->
                    PlayerResourcePackStatusEvent.Status.LOADED;
            case FAILED_DOWNLOAD, FAILED_RELOAD, INVALID_URL ->
                    PlayerResourcePackStatusEvent.Status.FAILED;
            default -> PlayerResourcePackStatusEvent.Status.FAILED;
        };
    }
}
