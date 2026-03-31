package com.axiommc.api.event.connection;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;

/**
 * Fired before a player logs in.
 * Can be used to reject connections or modify player data.
 */
public class PreLoginEvent extends Event implements Cancellable {

    private final String playerName;
    private boolean cancelled = false;
    private String denialReason;

    public PreLoginEvent(String playerName) {
        this.playerName = playerName;
    }

    public String username() {
        return playerName;
    }

    @Override
    public boolean isCancelled() {
        return cancelled;
    }

    @Override
    public void setCancelled(boolean cancelled) {
        this.cancelled = cancelled;
    }

    public String denialReason() {
        return denialReason;
    }

    public void deny(String reason) {
        this.cancelled = true;
        this.denialReason = reason;
    }
}
