package com.axiommc.api.event.player;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/** Fired when a player shears an entity. */
public final class PlayerShearEvent extends Event implements Cancellable {

    private final Player player;
    private final Entity entity;
    private boolean cancelled = false;

    public PlayerShearEvent(Player player, Entity entity) {
        this.player = player;
        this.entity = entity;
    }

    public Player player() {
        return player;
    }

    public Entity entity() {
        return entity;
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
