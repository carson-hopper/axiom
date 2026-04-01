package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.Server;

/**
 * Fired after a player has fully connected to a server.
 */
public class ServerPostConnectEvent extends Event {

    private final Player player;
    private final Server server;

    public ServerPostConnectEvent(Player player, Server server) {
        this.player = player;
        this.server = server;
    }

    public Player player() {
        return player;
    }

    public Server server() {
        return server;
    }
}
