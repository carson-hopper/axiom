package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired before a player command is processed. Cancel to prevent execution.
 */
public class PlayerCommandEvent extends Event implements Cancellable {

    private final Player player;
    private final String command;
    private boolean cancelled = false;

    public PlayerCommandEvent(Player player, String command) {
        this.player = player;
        this.command = command;
    }

    public Player player() {
        return player;
    }

    public String command() {
        return command;
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
