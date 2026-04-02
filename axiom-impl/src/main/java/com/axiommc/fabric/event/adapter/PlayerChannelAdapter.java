package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerClientBrandEvent;
import com.axiommc.api.event.player.PlayerPluginChannelEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.networking.v1.ClientboundPlayChannelEvents;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerClientBrandEvent}, {@link PlayerPluginChannelEvent.Register},
 * and {@link PlayerPluginChannelEvent.Unregister}.
 */
public class PlayerChannelAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerChannelAdapter.eventBus = eventBus;
        PlayerChannelAdapter.playerProvider = playerProvider;

        ClientboundPlayChannelEvents.REGISTER.register((handler, sender, server, channels) -> {
            ServerPlayer serverPlayer = handler.getPlayer();
            FabricPlayer player = new FabricPlayer(serverPlayer);
            for (Identifier channel : channels) {
                try {
                    eventBus.publish(new PlayerPluginChannelEvent.Register(
                            player, channel.toString()));
                } catch (Exception exception) {
                    Axiom.logger().debug(
                            "Error firing PlayerPluginChannelEvent.Register",
                            exception);
                }
            }
        });

        ClientboundPlayChannelEvents.UNREGISTER.register((handler, sender, server, channels) -> {
            ServerPlayer serverPlayer = handler.getPlayer();
            FabricPlayer player = new FabricPlayer(serverPlayer);
            for (Identifier channel : channels) {
                try {
                    eventBus.publish(new PlayerPluginChannelEvent.Unregister(
                            player, channel.toString()));
                } catch (Exception exception) {
                    Axiom.logger().debug(
                            "Error firing PlayerPluginChannelEvent.Unregister",
                            exception);
                }
            }
        });
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
            Axiom.logger().debug("Error firing PlayerClientBrandEvent", exception);
        }
    }
}
