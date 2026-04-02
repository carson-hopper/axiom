package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player swaps items between hands.
 */
public class PlayerSwapHandItemsEvent extends Event implements Cancellable {

    private final Player player;
    private boolean cancelled = false;

    public PlayerSwapHandItemsEvent(Player player) {
        this.player = player;
    }

    public Player player() {
        return player;
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
