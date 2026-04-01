package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.player.PlayerLeaveEvent;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.message.v1.ServerMessageEvents;
import net.fabricmc.fabric.api.networking.v1.ServerPlayConnectionEvents;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.contents.TranslatableContents;
import com.axiommc.fabric.Axiom;

/**
 * Fires PlayerJoinEvent and PlayerLeaveEvent using Fabric's connection callbacks.
 * Suppresses the default join/leave chat messages.
 */
public class PlayerConnectionAdapter implements FabricEventAdapter {

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerPlayConnectionEvents.JOIN.register((handler, sender, server) -> {
            try {
                var serverPlayer = handler.getPlayer();
                var player = playerProvider.player(serverPlayer.getUUID())
                        .orElseGet(() -> new FabricPlayer(serverPlayer));
                eventBus.publish(new PlayerJoinEvent(player));
            } catch (Exception e) {
                Axiom.logger().debug("Error firing PlayerJoinEvent", e);
            }
        });

        ServerPlayConnectionEvents.DISCONNECT.register((handler, server) -> {
            try {
                var serverPlayer = handler.getPlayer();
                var player = new FabricPlayer(serverPlayer);
                Axiom.logger().debug("Firing PlayerLeaveEvent for {}", player.name());
                eventBus.publish(new PlayerLeaveEvent(player));
            } catch (Exception e) {
                Axiom.logger().debug("Error firing PlayerLeaveEvent", e);
            }
        });

        // Suppress default join/leave messages
        ServerMessageEvents.ALLOW_GAME_MESSAGE.register((server, message, overlay) -> {
            if (message.getContents() instanceof TranslatableContents translatable) {
                String key = translatable.getKey();
                if (key.startsWith("multiplayer.player.joined") || key.startsWith("multiplayer.player.left")) {
                    return false;
                }
            }
            return true;
        });
    }
}
