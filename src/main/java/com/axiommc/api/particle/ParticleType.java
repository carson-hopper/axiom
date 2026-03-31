package com.axiommc.api.particle;

public interface ParticleType {
    /** Minecraft namespaced key, e.g. {@code "minecraft:flame"}. */
    String key();

    /**
     * The required {@link ParticleData} subclass, or {@code null} if this
     * particle type takes no data.
     */
    Class<?> dataType();
}
