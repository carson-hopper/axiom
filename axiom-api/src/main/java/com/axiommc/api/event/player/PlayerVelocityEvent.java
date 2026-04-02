package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Player;

/** Fired when a player's velocity is changed (e.g. knockback). */
public final class PlayerVelocityEvent extends Event implements Cancellable {

    private final Player player;
    private final Vector3 velocity;
    private boolean cancelled = false;

    public PlayerVelocityEvent(Player player, Vector3 velocity) {
        this.player = player;
        this.velocity = velocity;
    }

    public Player player() {
        return player;
    }

    public Vector3 velocity() {
        return velocity;
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
