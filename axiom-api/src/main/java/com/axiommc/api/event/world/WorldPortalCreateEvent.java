package com.axiommc.api.event.world;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;

/**
 * Fired when a portal is created in the world.
 */
public class WorldPortalCreateEvent extends Event implements Cancellable {

    private final World world;
    private final Location location;
    private final String reason;
    private boolean cancelled = false;

    public WorldPortalCreateEvent(World world, Location location, String reason) {
        this.world = world;
        this.location = location;
        this.reason = reason;
    }

    public World world() {
        return world;
    }

    public Location location() {
        return location;
    }

    public String reason() {
        return reason;
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
