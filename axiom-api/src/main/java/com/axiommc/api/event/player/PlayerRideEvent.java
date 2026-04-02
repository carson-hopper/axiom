package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player mounts or dismounts an entity.
 *
 * <p>Subtypes: {@link Mount}, {@link Dismount}.
 */
public class PlayerRideEvent {

    private PlayerRideEvent() {}

    /** Fired when a player starts riding an entity. */
    public static class Mount extends Event implements Cancellable {

        private final Player player;
        private final int entityId;
        private boolean cancelled = false;

        public Mount(Player player, int entityId) {
            this.player = player;
            this.entityId = entityId;
        }

        public Player player() {
            return player;
        }

        public int entityId() {
            return entityId;
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
        private final int entityId;

        public Dismount(Player player, int entityId) {
            this.player = player;
            this.entityId = entityId;
        }

        public Player player() {
            return player;
        }

        public int entityId() {
            return entityId;
        }
    }
}
