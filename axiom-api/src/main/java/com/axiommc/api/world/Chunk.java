package com.axiommc.api.world;

import com.axiommc.api.math.Vector3;

public interface Chunk {

    /** Chunk X coordinate in chunk space (block X / 16). */
    int x();

    /** Chunk Z coordinate in chunk space (block Z / 16). */
    int z();

    /** The world this chunk belongs to. */
    World world();

    /** Whether this chunk is currently loaded in memory. */
    boolean loaded();

    void load();

    void unload();

    /**
     * Returns the biome at the given block coordinates.
     * Biomes are sampled at 4-block resolution since 1.18 (3-D biomes).
     */
    Biome biomeAt(int x, int y, int z);

    default Biome biomeAt(Vector3 position) {
        return biomeAt((int) position.x(), (int) position.y(), (int) position.z());
    }

    void biome(Biome biome, Vector3 position);
}
