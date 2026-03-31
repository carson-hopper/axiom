package com.axiommc.api.world.block;

import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.Biome;
import com.axiommc.api.world.Chunk;
import com.axiommc.api.world.World;

public interface Block {

    /** Block-coordinate position (values are integer-equivalent). */
    Vector3 position();

    World world();

    /** Block type. Returns {@link Material#UNKNOWN} for unrecognised registry IDs. */
    Material type();

    /** Set the block type to the given material. */
    void type(Material type);

    /** Break this block naturally, dropping items. Returns true if successful, false otherwise. */
    boolean breakNaturally();

    /** Light level emitted by this block (0–15). */
    int lightLevel();

    /** Property values of this block state (e.g. {@code facing=north}, {@code powered=true}). */
    BlockState state();

    boolean solid();
    boolean liquid();
    boolean air();
    boolean opaque();

    /** Returns the block in the given direction. */
    Block relative(BlockFace face);

    /** Returns the block offset by the given delta. */
    Block relative(Vector3 delta);

    /** Returns the block at the given block-coordinate offset. */
    default Block relative(int dx, int dy, int dz) {
        return relative(new Vector3(dx, dy, dz));
    }

    /** Returns the chunk that contains this block. */
    default Chunk chunk() {
        int cx = world().toChunkCoord((int) position().x());
        int cz = world().toChunkCoord((int) position().z());
        return world().loadChunk(cx, cz);
    }

    /** Returns the biome at the block location. */
    default Biome biome() {
        return chunk().biomeAt(position());
    }
}
