package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.event.server.ServerStopEvent;
import com.axiommc.fabric.AxiomMod;
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
        ServerLifecycleEvents.SERVER_STARTED.register(server -> {
            LOGGER.debug("Firing ServerStartEvent");
            eventBus.publish(new ServerStartEvent(AxiomMod.getInstance().server()));
        });

        ServerLifecycleEvents.SERVER_STOPPING.register(server -> {
            LOGGER.debug("Firing ServerStopEvent");
            eventBus.publish(new ServerStopEvent(AxiomMod.getInstance().server()));
        });
    }
}
