package com.axiommc.api.math;

public record Vector2(float x, float y) {
    public static final Vector2 ZERO = new Vector2(0, 0);

    /** Alias for {@link #x()} — horizontal rotation. */
    public float yaw()   { return x; }

    /** Alias for {@link #y()} — vertical rotation. */
    public float pitch() { return y; }
}
