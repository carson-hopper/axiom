package com.axiommc.api.event.server;

import com.axiommc.api.event.Event;

/**
 * Fired when the server receives a list ping.
 */
public class ServerListPingEvent extends Event {

    private final String address;
    private final String motd;
    private final int maxPlayers;
    private final int onlinePlayers;

    public ServerListPingEvent(String address, String motd, int maxPlayers, int onlinePlayers) {
        this.address = address;
        this.motd = motd;
        this.maxPlayers = maxPlayers;
        this.onlinePlayers = onlinePlayers;
    }

    public String address() {
        return address;
    }

    public String motd() {
        return motd;
    }

    public int maxPlayers() {
        return maxPlayers;
    }

    public int onlinePlayers() {
        return onlinePlayers;
    }
}
