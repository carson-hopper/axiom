package com.axiommc.api.event.player;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player mounts or dismounts an entity.
 *
 * <ul>
 *   <li>{@link Mount} — player starts riding; cancellable
 *   <li>{@link Dismount} — player stops riding
 * </ul>
 */
public final class PlayerRideEvent {

    private PlayerRideEvent() {}

    /** Fired when a player starts riding an entity. */
    public static class Mount extends Event implements Cancellable {

        private final Player player;
        private final Entity entity;
        private boolean cancelled = false;

        public Mount(Player player, Entity entity) {
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

    /** Fired when a player stops riding an entity. */
    public static class Dismount extends Event {

        private final Player player;
        private final Entity entity;

        public Dismount(Player player, Entity entity) {
            this.player = player;
            this.entity = entity;
        }

        public Player player() {
            return player;
        }

        public Entity entity() {
            return entity;
        }
    }
}
