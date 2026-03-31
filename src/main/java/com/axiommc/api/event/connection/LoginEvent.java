package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player successfully logs in.
 */
public class LoginEvent extends Event {

    private final Player player;

    public LoginEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
    }
}
