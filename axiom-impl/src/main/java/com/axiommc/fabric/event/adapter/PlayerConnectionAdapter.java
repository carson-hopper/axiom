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
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Fires PlayerJoinEvent and PlayerLeaveEvent using Fabric's connection callbacks.
 * Suppresses the default join/leave chat messages.
 */
public class PlayerConnectionAdapter implements FabricEventAdapter {

    private static final Logger LOGGER = LoggerFactory.getLogger(PlayerConnectionAdapter.class);

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerPlayConnectionEvents.JOIN.register((handler, sender, server) -> {
            try {
                playerProvider.player(handler.getPlayer().getUUID()).ifPresent(player -> {
                    LOGGER.debug("Firing PlayerJoinEvent for {}", player.name());
                    eventBus.publish(new PlayerJoinEvent(player));
                });
            } catch (Exception e) {
                LOGGER.debug("Error firing PlayerJoinEvent", e);
            }
        });

        ServerPlayConnectionEvents.DISCONNECT.register((handler, server) -> {
            try {
                var serverPlayer = handler.getPlayer();
                var player = new FabricPlayer(serverPlayer);
                LOGGER.debug("Firing PlayerLeaveEvent for {}", player.name());
                eventBus.publish(new PlayerLeaveEvent(player));
            } catch (Exception e) {
                LOGGER.debug("Error firing PlayerLeaveEvent", e);
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
