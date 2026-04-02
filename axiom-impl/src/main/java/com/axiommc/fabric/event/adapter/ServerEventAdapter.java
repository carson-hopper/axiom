package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.server.ServerListPingEvent;
import com.axiommc.fabric.player.FabricPlayerProvider;

/**
 * Provides static hooks for server events fired from mixins
 * (e.g. ServerListPingEvent from ServerStatusPacketListenerMixin).
 */
public class ServerEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus bus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        bus = eventBus;
    }

    /**
     * Called from ServerStatusPacketListenerMixin when a status request is handled.
     */
    public static void onServerListPing(
        String address, String motd, int maxPlayers, int onlinePlayers) {
        if (bus == null) {
            return;
        }
        bus.publish(new ServerListPingEvent(address, motd, maxPlayers, onlinePlayers));
    }
}
