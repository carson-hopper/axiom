package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired after a player has fully logged in and joined the server.
 */
public class PostLoginEvent extends Event {

    private final Player player;

    public PostLoginEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
    }
}
