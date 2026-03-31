package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;

/**
 * Fired when a player initiates a connection handshake.
 */
public class ConnectionHandshakeEvent extends Event {

    private final String address;
    private final int port;

    public ConnectionHandshakeEvent(String address, int port) {
        this.address = address;
        this.port = port;
    }

    public String address() {
        return address;
    }

    public int port() {
        return port;
    }
}
