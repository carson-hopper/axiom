package com.axiommc.api.world;

import com.axiommc.api.entity.display.BlockDisplayEntity;
import com.axiommc.api.entity.display.BlockDisplaySpec;
import com.axiommc.api.entity.display.ItemDisplayEntity;
import com.axiommc.api.entity.display.ItemDisplaySpec;
import com.axiommc.api.entity.display.TextDisplayEntity;
import com.axiommc.api.entity.display.TextDisplaySpec;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.particle.ParticleEffect;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.block.Block;
import com.axiommc.api.world.block.Material;

import java.util.Collection;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;

/**
 * Represents a world on the server.
 *
 * <p>A world contains entities, blocks, players, and chunks. This interface
 * provides access to world properties, entities, blocks, and environmental
 * settings.
 */
public interface World {

    /**
     * Gets the name of this world.
     *
     * @return the world name
     */
    String name();

    /**
     * Gets the seed used to generate this world.
     *
     * @return the world seed
     */
    long seed();

    /**
     * Gets the dimension type of this world.
     *
     * @return the dimension
     */
    Dimension dimension();

    /**
     * Gets the minimum height of this world in blocks.
     *
     * @return the minimum Y coordinate
     */
    default int minHeight() {
        return dimension().minY();
    }

    /**
     * Gets the maximum height of this world in blocks.
     *
     * @return the maximum Y coordinate
     */
    default int maxHeight() {
        return dimension().maxY();
    }

    /**
     * Gets the logical height of this world (height span).
     *
     * @return the height span
     */
    default int logicalHeight() {
        return dimension().logicalHeight();
    }

    /**
     * Gets the current day-night cycle time.
     *
     * <p>The time cycles from 0 to 23999 ticks per day. 0 is sunrise,
     * 6000 is noon, 12000 is sunset, 18000 is midnight.
     *
     * @return the time in ticks
     */
    long time();

    /**
     * Sets the day-night cycle time.
     *
     * @param time the time in ticks (0-23999)
     */
    void time(long time);

    /**
     * Gets the absolute age of this world in ticks.
     *
     * <p>This value never resets and continuously increases.
     *
     * @return the world age in ticks
     */
    long fullTime();

    /**
     * Sets the absolute age of this world.
     *
     * @param time the new world age in ticks
     */
    void fullTime(long time);

    /**
     * Checks whether it is currently daytime in this world.
     *
     * @return true if daytime, false if nighttime
     */
    boolean dayTime();

    /**
     * Gets the current weather in this world.
     *
     * @return the weather
     */
    Weather weather();

    /**
     * Sets the weather in this world.
     *
     * @param weather the new weather
     */
    void weather(Weather weather);

    /**
     * Checks whether this world has clear weather.
     *
     * @return true if weather is clear
     */
    default boolean clearWeather() {
        return weather() == Weather.CLEAR;
    }

    /**
     * Gets the difficulty level of this world.
     *
     * @return the difficulty
     */
    Difficulty difficulty();

    /**
     * Sets the difficulty level of this world.
     *
     * @param difficulty the new difficulty
     */
    void difficulty(Difficulty difficulty);

    /**
     * Checks whether PvP is enabled in this world.
     *
     * @return true if PvP is enabled, false otherwise
     */
    boolean pvpEnabled();

    /**
     * Plays a sound at a specific position in this world.
     *
     * <p>The sound is heard by players within normal view distance of the
     * position.
     *
     * @param sound the sound to play
     * @param volume the volume (0.0 to 1.0+)
     * @param pitch the pitch (0.5 to 2.0 typical)
     * @param position the position where the sound originates
     */
    void playSound(SoundKey sound, float volume, float pitch, Vector3 position);

    /**
     * Gets the spawn location of this world.
     *
     * @return the spawn location
     */
    Location spawnLocation();

    /**
     * Gets the number of players in this world.
     *
     * @return the player count
     */
    int playerCount();

    /**
     * Gets all players in this world.
     *
     * @return a collection of players
     */
    Collection<? extends Player> players();

    /**
     * Gets a loaded chunk if it exists.
     *
     * @param chunkX the chunk X coordinate
     * @param chunkZ the chunk Z coordinate
     * @return an Optional containing the chunk if loaded, empty otherwise
     */
    Optional<Chunk> chunkAt(int chunkX, int chunkZ);

    /**
     * Loads a chunk, blocking until it is available.
     *
     * @param chunkX the chunk X coordinate
     * @param chunkZ the chunk Z coordinate
     * @return the loaded chunk
     */
    Chunk loadChunk(int chunkX, int chunkZ);

    /**
     * Loads a chunk asynchronously on the server thread.
     *
     * @param chunkX the chunk X coordinate
     * @param chunkZ the chunk Z coordinate
     * @return a future that completes when the chunk is loaded
     */
    CompletableFuture<Chunk> loadChunkAsync(int chunkX, int chunkZ);

    /**
     * Converts a block coordinate to its containing chunk coordinate.
     *
     * @param blockCoord the block coordinate
     * @return the chunk coordinate
     */
    default int toChunkCoord(int blockCoord) {
        return Math.floorDiv(blockCoord, 16);
    }

    /**
     * Gets the block at the specified coordinates.
     *
     * @param x the X coordinate
     * @param y the Y coordinate
     * @param z the Z coordinate
     * @return the block at this location
     */
    Block blockAt(int x, int y, int z);

    /**
     * Gets the block at the specified position.
     *
     * @param position the position
     * @return the block at this location
     */
    default Block blockAt(Vector3 position) {
        Objects.requireNonNull(position, "position");
        return blockAt(
                (int) Math.floor(position.x()),
                (int) Math.floor(position.y()),
                (int) Math.floor(position.z()));
    }

    /**
     * Gets the block at the specified location.
     *
     * @param location the location
     * @return the block at this location
     */
    default Block blockAt(Location location) {
        Objects.requireNonNull(location, "location");
        return blockAt(location.position());
    }

    /**
     * Gets the Y coordinate of the highest block at the specified X and Z.
     *
     * @param x the X coordinate
     * @param z the Z coordinate
     * @return the Y coordinate of the highest block
     */
    int highestBlockY(int x, int z);

    /**
     * Spawns a particle effect at the given location.
     *
     * <p>The particle is visible to nearby players within normal view distance,
     * unless {@link ParticleEffect#force()} is true.
     *
     * @param effect the particle effect
     * @param location the location to spawn at
     */
    void spawnParticle(ParticleEffect effect, Location location);

    /**
     * Spawns a text display entity at the given location.
     *
     * <p>If the spec has a TTL (time-to-live) greater than zero, the entity
     * is automatically removed after that many ticks.
     *
     * @param spec the display specification
     * @param location the location to spawn at
     * @return a handle to the spawned display entity
     */
    TextDisplayEntity spawnTextDisplay(TextDisplaySpec spec, Location location);

    /**
     * Spawns an item display entity at the given location.
     *
     * <p>If the spec has a TTL greater than zero, the entity is automatically
     * removed after that many ticks.
     *
     * @param spec the display specification
     * @param location the location to spawn at
     * @return a handle to the spawned display entity
     */
    ItemDisplayEntity spawnItemDisplay(ItemDisplaySpec spec, Location location);

    /**
     * Spawns a block display entity at the given location.
     *
     * <p>If the spec has a TTL greater than zero, the entity is automatically
     * removed after that many ticks.
     *
     * @param spec the display specification
     * @param location the location to spawn at
     * @return a handle to the spawned display entity
     */
    BlockDisplayEntity spawnBlockDisplay(BlockDisplaySpec spec, Location location);
}
