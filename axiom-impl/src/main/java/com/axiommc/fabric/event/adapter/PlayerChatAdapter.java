package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerChatEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.message.v1.ServerMessageEvents;

import java.util.Optional;

/**
 * Fires {@link PlayerChatEvent.Pre} and {@link PlayerChatEvent.Post}
 * using Fabric's message callbacks.
 */
public class PlayerChatAdapter implements FabricEventAdapter {

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerMessageEvents.ALLOW_CHAT_MESSAGE.register((message, sender, params) -> {
            try {
                Optional<FabricPlayer> player = playerProvider.player(sender.getUUID());
                if (player.isPresent()) {
                    String content = message.signedContent();
                    PlayerChatEvent.Pre event = new PlayerChatEvent.Pre(player.get(), content);
                    eventBus.publish(event);
                    return !event.isCancelled();
                }
            } catch (Exception exception) {
                Axiom.logger().debug("Error firing PlayerChatEvent.Pre", exception);
            }
            return true;
        });

        ServerMessageEvents.CHAT_MESSAGE.register((message, sender, params) -> {
            try {
                Optional<FabricPlayer> player = playerProvider.player(sender.getUUID());
                if (player.isPresent()) {
                    String content = message.signedContent();
                    eventBus.publish(new PlayerChatEvent.Post(player.get(), content));
                }
            } catch (Exception exception) {
                Axiom.logger().debug("Error firing PlayerChatEvent.Post", exception);
            }
        });
    }
}
