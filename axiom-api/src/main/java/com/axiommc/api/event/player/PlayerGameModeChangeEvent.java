package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.GameMode;
import com.axiommc.api.player.Player;

/**
 * Fired when a player's game mode changes.
 */
public class PlayerGameModeChangeEvent extends Event implements Cancellable {

    private final Player player;
    private final GameMode gameMode;
    private boolean cancelled = false;

    public PlayerGameModeChangeEvent(Player player, GameMode gameMode) {
        this.player = player;
        this.gameMode = gameMode;
    }

    public Player player() {
        return player;
    }

    public GameMode gameMode() {
        return gameMode;
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
