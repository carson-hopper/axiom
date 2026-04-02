package com.axiommc.api.event.server;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.Server;

/**
 * Event fired when the server starts.
 */
public class ServerStartEvent extends Event {

    private final Server server;

    public ServerStartEvent(Server server) {
        this.server = server;
    }

    public Server server() {
        return server;
    }
}
