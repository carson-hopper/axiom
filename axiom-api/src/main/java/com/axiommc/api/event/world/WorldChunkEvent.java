package com.axiommc.api.event.world;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.World;

/**
 * Events fired when chunks are loaded or unloaded.
 *
 * <p>Subtypes: {@link Load}, {@link Unload}.
 */
public class WorldChunkEvent {

    private WorldChunkEvent() {}

    /** Fired when a chunk is loaded. */
    public static class Load extends Event {

        private final World world;
        private final int chunkX;
        private final int chunkZ;

        public Load(World world, int chunkX, int chunkZ) {
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

    /** Fired when a chunk is unloaded. */
    public static class Unload extends Event {

        private final World world;
        private final int chunkX;
        private final int chunkZ;

        public Unload(World world, int chunkX, int chunkZ) {
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
