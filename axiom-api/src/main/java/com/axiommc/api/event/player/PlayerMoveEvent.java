package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Player;

/**
 * Fired when a player moves.
 */
public class PlayerMoveEvent extends Event implements Cancellable {

    private final Player player;
    private final Vector3 from;
    private final Vector3 to;
    private boolean cancelled = false;

    public PlayerMoveEvent(Player player, Vector3 from, Vector3 to) {
        this.player = player;
        this.from = from;
        this.to = to;
    }

    public Player player() {
        return player;
    }

    public Vector3 from() {
        return from;
    }

    public Vector3 to() {
        return to;
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
