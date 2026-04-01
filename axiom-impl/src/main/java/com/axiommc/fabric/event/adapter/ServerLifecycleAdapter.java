package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.event.server.ServerStopEvent;
import com.axiommc.api.world.Server;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerLifecycleEvents;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Fires ServerStartEvent and ServerStopEvent using Fabric lifecycle callbacks.
 */
public class ServerLifecycleAdapter implements FabricEventAdapter {

    private static final Logger LOGGER = LoggerFactory.getLogger(ServerLifecycleAdapter.class);

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerLifecycleEvents.SERVER_STARTED.register(mcServer -> {
            LOGGER.debug("Firing ServerStartEvent");
            String host = mcServer.getLocalIp().isEmpty() ? "localhost" : mcServer.getLocalIp();
            int port = mcServer.getPort();
            Server server = new Server("axiom-server", host, port);
            eventBus.publish(new ServerStartEvent(server));
        });

        ServerLifecycleEvents.SERVER_STOPPING.register(mcServer -> {
            LOGGER.debug("Firing ServerStopEvent");
            String host = mcServer.getLocalIp().isEmpty() ? "localhost" : mcServer.getLocalIp();
            int port = mcServer.getPort();
            Server server = new Server("axiom-server", host, port);
            eventBus.publish(new ServerStopEvent(server));
        });
    }
}
