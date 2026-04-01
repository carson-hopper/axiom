package com.axiommc.api.block;

import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.Biome;
import com.axiommc.api.world.Chunk;
import com.axiommc.api.world.World;

/**
 * Represents a single block in the world.
 *
 * <p>Provides access to the block's type, properties, and surrounding blocks.
 * Blocks are immutable; changes are made by setting the material or state.
 */
public interface Block {

    /**
     * Gets the block-coordinate position of this block.
     *
     * @return the position (coordinates are integer-equivalent)
     */
    Vector3 position();

    /**
     * Gets the world this block is in.
     *
     * @return the world
     */
    World world();

    /**
     * Gets the material type of this block.
     *
     * @return the block type (returns {@link Material#UNKNOWN} for unrecognized
     *         registry IDs)
     */
    Material type();

    /**
     * Sets the material type of this block.
     *
     * @param type the new block type
     */
    void type(Material type);

    /**
     * Breaks this block naturally, dropping any associated items.
     *
     * @return true if the break was successful, false otherwise
     */
    boolean breakNaturally();

    /**
     * Gets the light level emitted by this block.
     *
     * @return the light level (0-15)
     */
    int lightLevel();

    /**
     * Gets the block state properties of this block.
     *
     * <p>Block states include properties like {@code facing=north} or
     * {@code powered=true}.
     *
     * @return the block state
     */
    BlockState state();

    /**
     * Checks whether this block is solid.
     *
     * @return true if this block is solid
     */
    boolean solid();

    /**
     * Checks whether this block is a fluid.
     *
     * @return true if this block is a fluid (water/lava)
     */
    boolean liquid();

    /**
     * Checks whether this block is air.
     *
     * @return true if this block is air
     */
    boolean air();

    /**
     * Checks whether this block is opaque.
     *
     * @return true if this block is opaque
     */
    boolean opaque();

    /**
     * Gets the block adjacent to this block in the given direction.
     *
     * @param face the direction to check
     * @return the adjacent block
     */
    Block relative(BlockFace face);

    /**
     * Gets the block offset from this block by the given delta.
     *
     * @param delta the coordinate offset
     * @return the block at the offset
     */
    Block relative(Vector3 delta);

    /**
     * Gets the block offset from this block by the given coordinate delta.
     *
     * @param dx the X offset
     * @param dy the Y offset
     * @param dz the Z offset
     * @return the block at the offset
     */
    default Block relative(int dx, int dy, int dz) {
        return relative(new Vector3(dx, dy, dz));
    }

    /**
     * Gets the chunk containing this block.
     *
     * <p>This may load the chunk if not already loaded.
     *
     * @return the chunk containing this block
     */
    default Chunk chunk() {
        int cx = world().toChunkCoord((int) position().x());
        int cz = world().toChunkCoord((int) position().z());
        return world().loadChunk(cx, cz);
    }

    /**
     * Gets the biome at this block's location.
     *
     * @return the biome at this block
     */
    default Biome biome() {
        return chunk().biomeAt(position());
    }
}
