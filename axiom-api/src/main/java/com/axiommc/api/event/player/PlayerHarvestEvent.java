package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Fired when a player harvests a block.
 */
public class PlayerHarvestEvent extends Event implements Cancellable {

    private final Player player;
    private final Location blockLocation;
    private boolean cancelled = false;

    public PlayerHarvestEvent(Player player, Location blockLocation) {
        this.player = player;
        this.blockLocation = blockLocation;
    }

    public Player player() {
        return player;
    }

    public Location blockLocation() {
        return blockLocation;
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
