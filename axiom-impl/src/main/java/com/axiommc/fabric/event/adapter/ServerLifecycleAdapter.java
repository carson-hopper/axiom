package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.event.server.ServerStopEvent;
import com.axiommc.api.world.Server;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerLifecycleEvents;
import net.minecraft.server.MinecraftServer;
import com.axiommc.fabric.Axiom;

/**
 * Fires ServerStartEvent and ServerStopEvent using Fabric lifecycle callbacks.
 * Stores the MinecraftServer reference so AxiomMod can access it via event subscription.
 */
public class ServerLifecycleAdapter implements FabricEventAdapter {

    private static MinecraftServer currentServer;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        ServerLifecycleEvents.SERVER_STARTED.register(mcServer -> {
            currentServer = mcServer;
            Axiom.logger().debug("Firing ServerStartEvent");
            String host = mcServer.getLocalIp().isEmpty() ? "localhost" : mcServer.getLocalIp();
            int port = mcServer.getPort();
            Server server = new Server("axiom-server", host, port);
            eventBus.publish(new ServerStartEvent(server));
        });

        ServerLifecycleEvents.SERVER_STOPPING.register(mcServer -> {
            Axiom.logger().debug("Firing ServerStopEvent");
            String host = mcServer.getLocalIp().isEmpty() ? "localhost" : mcServer.getLocalIp();
            int port = mcServer.getPort();
            Server server = new Server("axiom-server", host, port);
            eventBus.publish(new ServerStopEvent(server));
            currentServer = null;
        });
    }

    /**
     * Returns the current MinecraftServer instance, available after ServerStartEvent fires.
     */
    public static MinecraftServer minecraftServer() {
        return currentServer;
    }
}
