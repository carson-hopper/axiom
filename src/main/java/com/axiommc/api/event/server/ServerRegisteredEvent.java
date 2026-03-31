package com.axiommc.api.event.server;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.Server;

/**
 * Fired when a new server is registered in the cluster.
 */
public class ServerRegisteredEvent extends Event {

    private final Server server;

    public ServerRegisteredEvent(Server server) {
        this.server = server;
    }

    public Server server() {
        return server;
    }
}
