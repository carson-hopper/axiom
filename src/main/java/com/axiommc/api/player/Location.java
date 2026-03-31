package com.axiommc.api.player;

import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;

/**
 * Represents a location in a world with position and rotation.
 *
 * <p>A location includes a world, 3D position (x, y, z), and rotation
 * (yaw and pitch). This record is immutable.
 *
 * @param world the world
 * @param position the 3D position
 * @param rotation the rotation (x = yaw, y = pitch)
 */
public record Location(World world, Vector3 position, Vector2 rotation) {

    /**
     * Calculates the distance between this location and another.
     *
     * <p>The distance is computed only from the positions; the world
     * and rotation are ignored.
     *
     * @param other the other location
     * @return the distance between the positions
     */
    public double distance(Location other) {
        return position.distance(other.position);
    }

    /**
     * Calculates the distance between this location's position and a vector.
     *
     * @param other the other position
     * @return the distance
     */
    public double distance(Vector3 other) {
        return position.distance(other);
    }

    /**
     * Checks whether this location is within a certain radius of another.
     *
     * <p>Locations must be in the same world to be considered close.
     *
     * @param other the other location
     * @param radius the radius to check
     * @return true if within the radius and in the same world
     */
    public boolean isCloseTo(Location other, double radius) {
        if (!world.equals(other.world)) {
            return false;
        }
        return position.isCloseTo(other.position, radius);
    }

    /**
     * Checks whether this location's position is within a certain radius.
     *
     * @param other the other position
     * @param radius the radius to check
     * @return true if within the radius
     */
    public boolean isCloseTo(Vector3 other, double radius) {
        return position.isCloseTo(other, radius);
    }
}
