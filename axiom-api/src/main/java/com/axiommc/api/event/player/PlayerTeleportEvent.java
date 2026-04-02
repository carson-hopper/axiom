package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Fired when a player is teleported.
 */
public class PlayerTeleportEvent extends Event implements Cancellable {

    private final Player player;
    private final Location from;
    private final Location to;
    private boolean cancelled = false;

    public PlayerTeleportEvent(Player player, Location from, Location to) {
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

    @Override
    public boolean isCancelled() {
        return cancelled;
    }

    @Override
    public void cancelled(boolean cancelled) {
        this.cancelled = cancelled;
    }
}
