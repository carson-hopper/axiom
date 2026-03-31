package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player's client brand is received.
 */
public class PlayerClientBrandEvent extends Event {

    private final Player player;
    private final String brand;

    public PlayerClientBrandEvent(Player player, String brand) {
        this.player = player;
        this.brand = brand;
    }

    public Player player() {
        return player;
    }

    public String brand() {
        return brand;
    }
}
