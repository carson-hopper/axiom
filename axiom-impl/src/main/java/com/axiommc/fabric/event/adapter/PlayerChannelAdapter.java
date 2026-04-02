package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerChannelRegisterEvent;
import com.axiommc.api.event.player.PlayerChannelUnregisterEvent;
import com.axiommc.api.event.player.PlayerClientBrandEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerClientBrandEvent}, {@link PlayerChannelRegisterEvent},
 * and {@link PlayerChannelUnregisterEvent}.
 * Called from {@code ServerCommonPacketListenerMixin}.
 */
public class PlayerChannelAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerChannelAdapter.eventBus = eventBus;
        PlayerChannelAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when the client sends its brand string.
     */
    public static void onClientBrand(ServerPlayer serverPlayer, String brand) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerClientBrandEvent(player, brand));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerClientBrandEvent", exception);
        }
    }

    /**
     * Called when the client registers a plugin channel.
     */
    public static void onChannelRegister(ServerPlayer serverPlayer,
                                         String channel) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(
                    new PlayerChannelRegisterEvent(player, channel));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerChannelRegisterEvent", exception);
        }
    }

    /**
     * Called when the client unregisters a plugin channel.
     */
    public static void onChannelUnregister(ServerPlayer serverPlayer,
                                           String channel) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(
                    new PlayerChannelUnregisterEvent(player, channel));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerChannelUnregisterEvent",
                    exception);
        }
    }
}
