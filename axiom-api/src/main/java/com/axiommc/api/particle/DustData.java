package com.axiommc.api.particle;

import java.util.Objects;

public final class DustData extends ParticleData {
    private final Color color;
    private final float size;

    public DustData(Color color, float size) {
        this.color = Objects.requireNonNull(color, "color");
        this.size = size;
    }

    public Color color() {
        return color;
    }

    public float size() {
        return size;
    }
}
