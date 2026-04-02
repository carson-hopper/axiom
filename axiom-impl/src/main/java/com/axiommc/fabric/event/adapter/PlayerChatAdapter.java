package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerChatEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import java.util.Optional;
import net.fabricmc.fabric.api.message.v1.ServerMessageEvents;

/**
 * Fires PlayerChatEvent using Fabric's message callbacks.
 * Supports cancellation via the Cancellable interface.
 */
public class PlayerChatAdapter implements FabricEventAdapter {

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerMessageEvents.ALLOW_CHAT_MESSAGE.register((message, sender, params) -> {
            try {
                Optional<FabricPlayer> player = playerProvider.player(sender.getUUID());
                if (player.isPresent()) {
                    String content = message.signedContent();
                    PlayerChatEvent event = new PlayerChatEvent(player.get(), content);
                    eventBus.publish(event);
                    return !event.isCancelled();
                }
            } catch (Exception exception) {
                Axiom.logger().debug("Error firing PlayerChatEvent", exception);
            }
            return true;
        });
    }
}
