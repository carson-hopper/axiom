package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player throws an egg.
 */
public class PlayerEggThrowEvent extends Event implements Cancellable {

    private final Player player;
    private final boolean hatching;
    private boolean cancelled = false;

    public PlayerEggThrowEvent(Player player, boolean hatching) {
        this.player = player;
        this.hatching = hatching;
    }

    public Player player() {
        return player;
    }

    public boolean hatching() {
        return hatching;
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
