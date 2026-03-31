package com.axiommc.api.event.server;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.Server;

/**
 * Fired when a server is unregistered from the cluster.
 */
public class ServerUnregisteredEvent extends Event {

    private final Server server;

    public ServerUnregisteredEvent(Server server) {
        this.server = server;
    }

    public Server server() {
        return server;
    }
}
