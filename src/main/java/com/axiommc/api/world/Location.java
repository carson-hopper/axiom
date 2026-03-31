package com.axiommc.api.world;

import com.axiommc.api.math.Vector3;

/**
 * An immutable world position expressed in block coordinates.
 *
 * @param position  block coordinates
 * @param worldName the Minecraft world key (e.g. {@code "minecraft:overworld"})
 */
public record Location(Vector3 position, String worldName) {

    /**
     * Returns a new Location offset by the given deltas.
     */
    public Location offset(double dx, double dy, double dz) {
        return new Location(new Vector3(position.x() + dx, position.y() + dy, position.z() + dz), worldName);
    }
}
