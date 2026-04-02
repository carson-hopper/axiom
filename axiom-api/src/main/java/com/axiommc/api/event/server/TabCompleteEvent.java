package com.axiommc.api.event.server;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import java.util.List;

/**
 * Fired when a player tab-completes a command.
 */
public class TabCompleteEvent extends Event implements Cancellable {

    private final Player player;
    private final String buffer;
    private final List<String> completions;
    private boolean cancelled = false;

    public TabCompleteEvent(Player player, String buffer, List<String> completions) {
        this.player = player;
        this.buffer = buffer;
        this.completions = completions;
    }

    public Player player() {
        return player;
    }

    public String buffer() {
        return buffer;
    }

    public List<String> completions() {
        return completions;
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
