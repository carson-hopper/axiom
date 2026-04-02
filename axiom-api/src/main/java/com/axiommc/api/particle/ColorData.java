package com.axiommc.api.particle;

public final class ColorData extends ParticleData {
    private final int argb;

    public ColorData(int argb) {
        this.argb = argb;
    }

    public static ColorData of(Color color) {
        return new ColorData(0xFF000000 | (color.r() << 16) | (color.g() << 8) | color.b());
    }

    public int argb() {
        return argb;
    }
}
