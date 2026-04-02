package com.axiommc.api.event.player;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player is fishing.
 */
public class PlayerFishEvent extends Event implements Cancellable {

    private final Player player;
    private final Entity caught;
    private final String state;
    private boolean cancelled = false;

    public PlayerFishEvent(Player player, Entity caught, String state) {
        this.player = player;
        this.caught = caught;
        this.state = state;
    }

    public Player player() {
        return player;
    }

    public Entity caught() {
        return caught;
    }

    public String state() {
        return state;
    }

    @Override
    public boolean isCancelled() {
        return cancelled;
    }

    @Override
    public void cancelled(boolean cancelled) {
        this.cancelled = cancelled;
    }
}
