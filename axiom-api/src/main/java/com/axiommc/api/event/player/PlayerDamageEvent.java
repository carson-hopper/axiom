package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player takes damage.
 */
public class PlayerDamageEvent extends Event implements Cancellable {

    private final Player player;
    private final double damage;
    private final String cause;
    private boolean cancelled = false;

    public PlayerDamageEvent(Player player, double damage, String cause) {
        this.player = player;
        this.damage = damage;
        this.cause = cause;
    }

    public Player player() {
        return player;
    }

    public double damage() {
        return damage;
    }

    public String cause() {
        return cause;
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
