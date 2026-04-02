package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player interacts with an entity.
 */
public class PlayerInteractEntityEvent extends Event implements Cancellable {

    private final Player player;
    private final int entityId;
    private final boolean mainHand;
    private boolean cancelled = false;

    public PlayerInteractEntityEvent(Player player, int entityId, boolean mainHand) {
        this.player = player;
        this.entityId = entityId;
        this.mainHand = mainHand;
    }

    public Player player() {
        return player;
    }

    public int entityId() {
        return entityId;
    }

    public boolean mainHand() {
        return mainHand;
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
