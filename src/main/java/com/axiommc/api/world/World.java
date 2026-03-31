package com.axiommc.api.world;

import com.axiommc.api.entity.display.*;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.particle.ParticleEffect;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.block.Block;
import com.axiommc.api.world.block.Material;

import java.util.Collection;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;

public interface World {

    // Identity
    String name();
    long seed();
    Dimension dimension();

    // Height convenience — derived from Dimension
    default int minHeight() {
        return dimension().minY();
    }

    default int maxHeight() {
        return dimension().maxY();
    }

    default int logicalHeight() {
        return dimension().logicalHeight();
    }

    // Time (0–23999 per day cycle)
    long time();
    void time(long time);

    /** Absolute world age in ticks — never resets. */
    long fullTime();
    void fullTime(long time);

    boolean dayTime();

    // Weather
    Weather weather();
    void weather(Weather weather);

    default boolean clearWeather() {
        return weather() == Weather.CLEAR;
    }

    // Environment
    Difficulty difficulty();
    void difficulty(Difficulty difficulty);
    boolean pvpEnabled();

    // Spawn
    Location spawnLocation();

    // Players
    int playerCount();
    Collection<? extends Player> players();

    // Chunks

    /** Returns the chunk if already loaded, empty otherwise. */
    Optional<Chunk> chunkAt(int chunkX, int chunkZ);

    /** Loads and returns the chunk at the given chunk coordinates, blocking until complete. */
    Chunk loadChunk(int chunkX, int chunkZ);

    /** Loads the chunk asynchronously, completing on the server thread when ready. */
    CompletableFuture<Chunk> loadChunkAsync(int chunkX, int chunkZ);

    /** Converts a block coordinate to its containing chunk coordinate. */
    default int toChunkCoord(int blockCoord) {
        return Math.floorDiv(blockCoord, 16);
    }

    // Blocks

    Block blockAt(int x, int y, int z);
    default Block blockAt(Vector3 position) {
        Objects.requireNonNull(position, "position");
        return blockAt(
                (int) Math.floor(position.x()),
                (int) Math.floor(position.y()),
                (int) Math.floor(position.z()));
    }
    default Block blockAt(Location location) {
        Objects.requireNonNull(location, "location");
        return blockAt(location.position());
    }

    Optional<Biome> biomeAt(int x, int y, int z);
    int highestBlockY(int x, int z);
    void setBlock(int x, int y, int z, Material type);

    // Particles

    /**
     * Spawns a particle effect at the given location, visible to nearby players
     * within normal view distance (or all players if {@link ParticleEffect#force()} is true).
     */
    void spawnParticle(ParticleEffect effect, Location location);

    // Display entities

    /**
     * Spawns a text display entity at the given location and returns a typed handle.
     * If {@code spec.ttl()} is greater than zero the entity is automatically removed
     * after that many ticks.
     */
    TextDisplayEntity  spawnTextDisplay (TextDisplaySpec  spec, Location location);

    /**
     * Spawns an item display entity at the given location and returns a typed handle.
     * If {@code spec.ttl()} is greater than zero the entity is automatically removed
     * after that many ticks.
     */
    ItemDisplayEntity  spawnItemDisplay (ItemDisplaySpec  spec, Location location);

    /**
     * Spawns a block display entity at the given location and returns a typed handle.
     * If {@code spec.ttl()} is greater than zero the entity is automatically removed
     * after that many ticks.
     */
    BlockDisplayEntity spawnBlockDisplay(BlockDisplaySpec spec, Location location);
}
