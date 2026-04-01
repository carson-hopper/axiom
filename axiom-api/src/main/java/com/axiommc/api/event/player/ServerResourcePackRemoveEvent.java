package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a resource pack is removed from a player.
 */
public class ServerResourcePackRemoveEvent extends Event {

    private final Player player;

    public ServerResourcePackRemoveEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
    }
}
