package com.axiommc.api.particle;

import java.util.Objects;

public final class DustTransitionData extends ParticleData {
    private final Color fromColor;
    private final Color toColor;
    private final float size;

    public DustTransitionData(Color fromColor, Color toColor, float size) {
        this.fromColor = Objects.requireNonNull(fromColor, "fromColor");
        this.toColor = Objects.requireNonNull(toColor, "toColor");
        this.size = size;
    }

    public Color fromColor() {
        return fromColor;
    }

    public Color toColor() {
        return toColor;
    }

    public float size() {
        return size;
    }
}
