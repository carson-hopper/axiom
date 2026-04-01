package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerChatEvent;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.message.v1.ServerMessageEvents;
import com.axiommc.fabric.Axiom;

/**
 * Fires PlayerChatEvent using Fabric's message callbacks.
 * Supports cancellation via the Cancellable interface.
 */
public class PlayerChatAdapter implements FabricEventAdapter {

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerMessageEvents.ALLOW_CHAT_MESSAGE.register((message, sender, params) -> {
            try {
                var player = playerProvider.player(sender.getUUID());
                if (player.isPresent()) {
                    String content = message.signedContent();
                    PlayerChatEvent event = new PlayerChatEvent(player.get(), content);
                    eventBus.publish(event);
                    return !event.isCancelled();
                }
            } catch (Exception e) {
                Axiom.logger().debug("Error firing PlayerChatEvent", e);
            }
            return true;
        });
    }
}
