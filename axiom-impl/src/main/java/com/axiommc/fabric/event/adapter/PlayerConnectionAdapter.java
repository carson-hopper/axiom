package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.player.PlayerLeaveEvent;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.message.v1.ServerMessageEvents;
import net.fabricmc.fabric.api.networking.v1.ServerPlayConnectionEvents;
import net.minecraft.network.chat.contents.TranslatableContents;
import net.minecraft.server.level.ServerPlayer;
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
                ServerPlayer serverPlayer = handler.getPlayer();
                FabricPlayer player = playerProvider.player(serverPlayer.getUUID())
                        .orElseGet(() -> new FabricPlayer(serverPlayer));
                eventBus.publish(new PlayerJoinEvent(player));
            } catch (Exception exception) {

            }
        });

        ServerPlayConnectionEvents.DISCONNECT.register((handler, server) -> {
            try {
                ServerPlayer serverPlayer = handler.getPlayer();
                FabricPlayer player = new FabricPlayer(serverPlayer);
                Axiom.logger().debug("Firing PlayerLeaveEvent for {}", player.name());
                eventBus.publish(new PlayerLeaveEvent(player));
            } catch (Exception exception) {

            }
        });

        // Suppress default join/leave messages
        ServerMessageEvents.ALLOW_GAME_MESSAGE.register((server, message, overlay) -> {
            if (message.getContents() instanceof TranslatableContents translatable) {
                String key = translatable.getKey();
                return !key.startsWith("multiplayer.player.joined") && !key.startsWith("multiplayer.player.left");
            }
            return true;
        });
    }
}
