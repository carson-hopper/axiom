package com.axiommc.api.entity.display;

import com.axiommc.api.entity.Entity;

/**
 * A persistent display entity spawned in a world. Extends {@link Entity}
 * for position, UUID, and other common entity state.
 */
public interface DisplayEntity extends Entity {

    /** Removes this entity from the world immediately. Safe to call multiple times. */
    void remove();

    /** Returns {@code true} after {@link #remove()} has been called or the TTL has expired. */
    boolean isRemoved();

}
