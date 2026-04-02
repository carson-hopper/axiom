package com.axiommc.api.math;

/**
 * An immutable 2D vector (x, y).
 *
 * <p>Used for screen coordinates, rotations (yaw/pitch), and 2D positions.
 *
 * @param x the X component
 * @param y the Y component
 */
public record Vector2(float x, float y) {

    /** Vector with all components set to zero. */
    public static final Vector2 ZERO = new Vector2(0, 0);

    /**
     * Creates a Vector2 from double values.
     *
     * @param x the X component
     * @param y the Y component
     * @return a new Vector2
     */
    public static Vector2 of(float x, float y) {
        return new Vector2(x, y);
    }

    /** Alias for {@link #x()} — horizontal rotation. */
    public float yaw() {
        return x;
    }

    /** Alias for {@link #y()} — vertical rotation. */
    public float pitch() {
        return y;
    }

    /** Alias for {@link #x()} — width dimension. */
    public float width() {
        return x;
    }

    /** Alias for {@link #y()} — height dimension. */
    public float height() {
        return y;
    }

    /**
     * Adds another vector to this vector.
     *
     * @param other the vector to add
     * @return a new vector representing the sum
     */
    public Vector2 add(Vector2 other) {
        return new Vector2(x + other.x, y + other.y);
    }

    /**
     * Subtracts another vector from this vector.
     *
     * @param other the vector to subtract
     * @return a new vector representing the difference
     */
    public Vector2 subtract(Vector2 other) {
        return new Vector2(x - other.x, y - other.y);
    }

    /**
     * Multiplies this vector by a scalar factor.
     *
     * @param factor the scaling factor
     * @return a new scaled vector
     */
    public Vector2 multiply(float factor) {
        return new Vector2(x * factor, y * factor);
    }
}
