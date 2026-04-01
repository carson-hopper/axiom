package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player sends a chat message.
 */
public class PlayerChatEvent extends Event implements Cancellable {

    private final Player player;
    private String message;
    private boolean cancelled = false;

    public PlayerChatEvent(Player player, String message) {
        this.player = player;
        this.message = message;
    }

    public Player player() {
        return player;
    }

    public String message() {
        return message;
    }

    public void message(String message) {
        this.message = message;
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
