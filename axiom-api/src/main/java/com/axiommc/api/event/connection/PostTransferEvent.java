package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.Server;

/**
 * Fired after a player has been transferred to another server.
 */
public class PostTransferEvent extends Event {

    private final Player player;
    private final Server targetServer;

    public PostTransferEvent(Player player, Server targetServer) {
        this.player = player;
        this.targetServer = targetServer;
    }

    public Player player() {
        return player;
    }

    public Server targetServer() {
        return targetServer;
    }
}
