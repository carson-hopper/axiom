package com.axiommc.api.event.server;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.Server;

/**
 * Event fired when the server stops.
 */
public class ServerStopEvent extends Event {

    private final Server server;

    public ServerStopEvent(Server server) {
        this.server = server;
    }

    public Server server() {
        return server;
    }
}
