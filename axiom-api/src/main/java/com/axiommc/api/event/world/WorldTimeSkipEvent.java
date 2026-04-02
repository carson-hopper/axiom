package com.axiommc.api.event.world;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.world.World;

/**
 * Fired when time is skipped in a world (e.g. sleeping or command).
 */
public class WorldTimeSkipEvent extends Event implements Cancellable {

    private final World world;
    private final long skipAmount;
    private final String reason;
    private boolean cancelled = false;

    public WorldTimeSkipEvent(World world, long skipAmount, String reason) {
        this.world = world;
        this.skipAmount = skipAmount;
        this.reason = reason;
    }

    public World world() {
        return world;
    }

    public long skipAmount() {
        return skipAmount;
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
