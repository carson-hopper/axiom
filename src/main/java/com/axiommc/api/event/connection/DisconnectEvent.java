package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player disconnects from the server.
 */
public class DisconnectEvent extends Event {

    private final Player player;

    public DisconnectEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
    }
}
