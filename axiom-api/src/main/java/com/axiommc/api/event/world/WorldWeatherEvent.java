package com.axiommc.api.event.world;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;

/**
 * Events fired when world weather conditions change.
 *
 * <ul>
 *   <li>{@link Change} — rain starts or stops; cancellable
 *   <li>{@link ThunderChange} — thunder starts or stops; cancellable
 *   <li>{@link LightningStrike} — lightning strikes; cancellable
 * </ul>
 */
public class WorldWeatherEvent {

    private WorldWeatherEvent() {}

    /** Fired when a world's weather changes. */
    public static class Change extends Event implements Cancellable {

        private final World world;
        private final boolean raining;
        private boolean cancelled = false;

        public Change(World world, boolean raining) {
            this.world = world;
            this.raining = raining;
        }

        public World world() {
            return world;
        }

        public boolean raining() {
            return raining;
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

    /** Fired when a world's thunder state changes. */
    public static class ThunderChange extends Event implements Cancellable {

        private final World world;
        private final boolean thundering;
        private boolean cancelled = false;

        public ThunderChange(World world, boolean thundering) {
            this.world = world;
            this.thundering = thundering;
        }

        public World world() {
            return world;
        }

        public boolean thundering() {
            return thundering;
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

    /** Fired when lightning strikes in a world. */
    public static class LightningStrike extends Event implements Cancellable {

        private final World world;
        private final Vector3 position;
        private boolean cancelled = false;

        public LightningStrike(World world, Vector3 position) {
            this.world = world;
            this.position = position;
        }

        public World world() {
            return world;
        }

        public Vector3 position() {
            return position;
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
}
