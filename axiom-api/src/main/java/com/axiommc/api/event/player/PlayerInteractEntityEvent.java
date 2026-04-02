package com.axiommc.api.event.player;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player interacts with an entity.
 */
public class PlayerInteractEntityEvent extends Event implements Cancellable {

    private final Player player;
    private final Entity entity;
    private final boolean mainHand;
    private boolean cancelled = false;

    public PlayerInteractEntityEvent(Player player, Entity entity, boolean mainHand) {
        this.player = player;
        this.entity = entity;
        this.mainHand = mainHand;
    }

    public Player player() {
        return player;
    }

    public Entity entity() {
        return entity;
    }

    public boolean isMainHand() {
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
