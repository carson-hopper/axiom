package com.axiommc.api.event.world;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.world.World;

/**
 * Events fired when chunks are loaded or unloaded.
 *
 * <ul>
 *   <li>{@link PreLoad} — before a chunk loads; cancellable
 *   <li>{@link PostLoad} — after a chunk has loaded
 *   <li>{@link PreUnload} — before a chunk unloads; cancellable
 *   <li>{@link PostUnload} — after a chunk has unloaded
 * </ul>
 */
public final class WorldChunkEvent {

    private WorldChunkEvent() {}

    /** Fired before a chunk is loaded. */
    public static class PreLoad extends Event implements Cancellable {

        private final World world;
        private final int chunkX;
        private final int chunkZ;
        private boolean cancelled = false;

        public PreLoad(World world, int chunkX, int chunkZ) {
            this.world = world;
            this.chunkX = chunkX;
            this.chunkZ = chunkZ;
        }

        public World world() {
            return world;
        }

        public int chunkX() {
            return chunkX;
        }

        public int chunkZ() {
            return chunkZ;
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

    /** Fired after a chunk has loaded. */
    public static class PostLoad extends Event {

        private final World world;
        private final int chunkX;
        private final int chunkZ;

        public PostLoad(World world, int chunkX, int chunkZ) {
            this.world = world;
            this.chunkX = chunkX;
            this.chunkZ = chunkZ;
        }

        public World world() {
            return world;
        }

        public int chunkX() {
            return chunkX;
        }

        public int chunkZ() {
            return chunkZ;
        }
    }

    /** Fired before a chunk is unloaded. */
    public static class PreUnload extends Event implements Cancellable {

        private final World world;
        private final int chunkX;
        private final int chunkZ;
        private boolean cancelled = false;

        public PreUnload(World world, int chunkX, int chunkZ) {
            this.world = world;
            this.chunkX = chunkX;
            this.chunkZ = chunkZ;
        }

        public World world() {
            return world;
        }

        public int chunkX() {
            return chunkX;
        }

        public int chunkZ() {
            return chunkZ;
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

    /** Fired after a chunk has unloaded. */
    public static class PostUnload extends Event {

        private final World world;
        private final int chunkX;
        private final int chunkZ;

        public PostUnload(World world, int chunkX, int chunkZ) {
            this.world = world;
            this.chunkX = chunkX;
            this.chunkZ = chunkZ;
        }

        public World world() {
            return world;
        }

        public int chunkX() {
            return chunkX;
        }

        public int chunkZ() {
            return chunkZ;
        }
    }
}
