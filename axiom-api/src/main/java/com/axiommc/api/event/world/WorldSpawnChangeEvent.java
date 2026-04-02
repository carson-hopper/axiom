package com.axiommc.api.event.world;

import com.axiommc.api.event.Event;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;

/**
 * Fired when a world's spawn point changes.
 */
public class WorldSpawnChangeEvent extends Event {

    private final World world;
    private final Vector3 oldSpawn;
    private final Vector3 newSpawn;

    public WorldSpawnChangeEvent(World world, Vector3 oldSpawn, Vector3 newSpawn) {
        this.world = world;
        this.oldSpawn = oldSpawn;
        this.newSpawn = newSpawn;
    }

    public World world() {
        return world;
    }

    public Vector3 oldSpawn() {
        return oldSpawn;
    }

    public Vector3 newSpawn() {
        return newSpawn;
    }
}
