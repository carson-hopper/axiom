package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player's position changes.
 *
 * <ul>
 *   <li>{@link Move} — player moved normally; cancellable
 *   <li>{@link Teleport} — player was teleported; cancellable
 *   <li>{@link WorldChange} — player changed worlds
 * </ul>
 */
public final class PlayerPositionEvent {

    private PlayerPositionEvent() {}

    /**
     * Fired when a player moves.
     */
    public static class Move extends Event implements Cancellable {

        private final Player player;
        private final Vector3 from;
        private final Vector3 to;
        private boolean cancelled = false;

        public Move(Player player, Vector3 from, Vector3 to) {
            this.player = player;
            this.from = from;
            this.to = to;
        }

        public Player player() {
            return player;
        }

        public Vector3 from() {
            return from;
        }

        public Vector3 to() {
            return to;
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

    /**
     * Fired when a player is teleported.
     */
    public static class Teleport extends Event implements Cancellable {

        private final Player player;
        private final Location from;
        private final Location to;
        private boolean cancelled = false;

        public Teleport(Player player, Location from, Location to) {
            this.player = player;
            this.from = from;
            this.to = to;
        }

        public Player player() {
            return player;
        }

        public Location from() {
            return from;
        }

        public Location to() {
            return to;
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

    /**
     * Fired when a player changes worlds.
     */
    public static class WorldChange extends Event {

        private final Player player;
        private final Location from;
        private final Location to;

        public WorldChange(Player player, Location from, Location to) {
            this.player = player;
            this.from = from;
            this.to = to;
        }

        public Player player() {
            return player;
        }

        public Location from() {
            return from;
        }

        public Location to() {
            return to;
        }
    }
}
