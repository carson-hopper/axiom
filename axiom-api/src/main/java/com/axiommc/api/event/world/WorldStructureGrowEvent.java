package com.axiommc.api.event.world;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;

/**
 * Fired when an organic structure grows (e.g. sapling to tree).
 */
public class WorldStructureGrowEvent extends Event implements Cancellable {

    private final World world;
    private final Location location;
    private final String structureType;
    private boolean cancelled = false;

    public WorldStructureGrowEvent(World world, Location location, String structureType) {
        this.world = world;
        this.location = location;
        this.structureType = structureType;
    }

    public World world() {
        return world;
    }

    public Location location() {
        return location;
    }

    public String structureType() {
        return structureType;
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
