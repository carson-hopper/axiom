package com.axiommc.api.math;

/**
 * An immutable 3D vector (x, y, z).
 *
 * <p>Used for positions, velocities, and directional offsets in 3D space.
 * This record is immutable; all operations return new instances.
 *
 * @param x the X component
 * @param y the Y component
 * @param z the Z component
 */
public record Vector3(double x, double y, double z) {

    /**
     * Vector with all components set to zero.
     */
    public static final Vector3 ZERO = new Vector3(0, 0, 0);

    /**
     * Adds another vector to this vector.
     *
     * @param other the vector to add
     * @return a new vector representing the sum
     */
    public Vector3 add(Vector3 other) {
        return new Vector3(x + other.x, y + other.y, z + other.z);
    }

    /**
     * Multiplies this vector by a scalar factor.
     *
     * @param factor the scaling factor
     * @return a new scaled vector
     */
    public Vector3 multiply(double factor) {
        return new Vector3(x * factor, y * factor, z * factor);
    }

    /**
     * Calculates the length (magnitude) of this vector.
     *
     * @return the length
     */
    public double length() {
        return Math.sqrt(x * x + y * y + z * z);
    }

    /**
     * Calculates the squared length of this vector.
     *
     * <p>Useful for comparison operations where the actual length is not needed,
     * avoiding the expensive square root operation.
     *
     * @return the squared length
     */
    public double lengthSquared() {
        return x * x + y * y + z * z;
    }

    /**
     * Calculates the Euclidean distance between this vector and another.
     *
     * @param other the other vector
     * @return the distance
     */
    public double distance(Vector3 other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }

    /**
     * Calculates the squared distance between this vector and another.
     *
     * <p>Useful for distance comparisons where the actual distance is not needed.
     *
     * @param other the other vector
     * @return the squared distance
     */
    public double distanceSquared(Vector3 other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    /**
     * Checks whether this vector is within a given radius of another.
     *
     * @param other the other vector
     * @param radius the radius to check
     * @return true if the distance is less than or equal to the radius
     */
    public boolean isCloseTo(Vector3 other, double radius) {
        return distance(other) <= radius;
    }
}
