package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayerProvider;

import java.util.UUID;

/**
 * Fires {@link PlayerJoinEvent.Init} when a player begins
 * the login handshake. Called from {@code ServerLoginPacketListenerMixin}.
 */
public class GameProfileAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        GameProfileAdapter.eventBus = eventBus;
    }

    /**
     * Called from the login mixin when a hello packet arrives.
     *
     * @param playerName the name from the login packet
     * @param profileId  the UUID from the login packet
     */
    public static void onGameProfileRequest(String playerName, UUID profileId) {
        if (eventBus == null) {
            return;
        }
        try {
            PlayerJoinEvent.Init event = new PlayerJoinEvent.Init(playerName);
            if (profileId != null) {
                event.uuid(profileId);
            }
            eventBus.publish(event);
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing GameProfileRequest", exception);
        }
    }
}
