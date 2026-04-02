package com.axiommc.api.event.world;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.World;

/**
 * Events fired during world lifecycle transitions.
 *
 * <p>Subtypes: {@link Load}, {@link Unload}, {@link Save}.
 */
public class WorldLifecycleEvent {

    private WorldLifecycleEvent() {}

    /** Fired when a world is loaded. */
    public static class Load extends Event {

        private final World world;

        public Load(World world) {
            this.world = world;
        }

        public World world() {
            return world;
        }
    }

    /** Fired when a world is unloaded. */
    public static class Unload extends Event {

        private final World world;

        public Unload(World world) {
            this.world = world;
        }

        public World world() {
            return world;
        }
    }

    /** Fired when a world is saved. */
    public static class Save extends Event {

        private final World world;

        public Save(World world) {
            this.world = world;
        }

        public World world() {
            return world;
        }
    }
}
