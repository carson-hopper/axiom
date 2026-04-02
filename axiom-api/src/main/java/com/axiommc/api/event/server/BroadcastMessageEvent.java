package com.axiommc.api.event.server;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;

/**
 * Fired when a broadcast message is sent to all players.
 */
public class BroadcastMessageEvent extends Event implements Cancellable {

    private final String message;
    private boolean cancelled = false;

    public BroadcastMessageEvent(String message) {
        this.message = message;
    }

    public String message() {
        return message;
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
