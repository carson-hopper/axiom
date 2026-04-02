package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Fired when a player changes worlds.
 */
public class PlayerChangedWorldEvent extends Event {

    private final Player player;
    private final Location from;
    private final Location to;

    public PlayerChangedWorldEvent(Player player, Location from, Location to) {
        this.player = player;
        this.from = from;
        this.to = to;
    }

    public Player player() {
        return player;
    }

    public Location from() {
        return from;
    }

    public Location to() {
        return to;
    }
}
