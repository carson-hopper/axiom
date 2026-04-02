package com.axiommc.api.particle;

public record Color(int r, int g, int b) {

    public Color {
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            throw new IllegalArgumentException("RGB values must be 0–255");
        }
    }

    public static Color of(int r, int g, int b) {
        return new Color(r, g, b);
    }

    public static final Color WHITE = of(255, 255, 255);
    public static final Color RED = of(255, 0, 0);
    public static final Color GREEN = of(0, 255, 0);
    public static final Color BLUE = of(0, 0, 255);
    public static final Color YELLOW = of(255, 255, 0);
    public static final Color AQUA = of(0, 255, 255);
    public static final Color BLACK = of(0, 0, 0);

    /** Returns the red channel normalised to [0, 1] for use in Minecraft's Vector3f. */
    public float rf() {
        return r / 255.0f;
    }
    /** Returns the green channel normalised to [0, 1]. */
    public float gf() {
        return g / 255.0f;
    }
    /** Returns the blue channel normalised to [0, 1]. */
    public float bf() {
        return b / 255.0f;
    }
}
