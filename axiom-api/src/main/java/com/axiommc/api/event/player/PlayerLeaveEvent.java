package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Event fired when a player leaves.
 */
public class PlayerLeaveEvent extends Event {
    private final Player player;

    public PlayerLeaveEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
    }
}
