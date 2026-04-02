package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player is kicked from the server.
 */
public class PlayerKickEvent extends Event {

    private final Player player;
    private final String reason;

    public PlayerKickEvent(Player player, String reason) {
        this.player = player;
        this.reason = reason;
    }

    public Player player() {
        return player;
    }

    public String reason() {
        return reason;
    }
}
