package com.axiommc.fabric.world;

import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.Biome;
import com.axiommc.api.world.BiomeWritable;
import com.axiommc.api.world.Chunk;
import com.axiommc.api.world.World;
import net.minecraft.server.level.FullChunkStatus;
import net.minecraft.world.level.chunk.LevelChunk;
import net.minecraft.world.level.chunk.status.ChunkStatus;

public record FabricChunk(LevelChunk levelChunk, FabricWorld fabricWorld) implements Chunk {

    // ============================================================
    // Position & World
    // ============================================================

    @Override
    public int x() {
        return levelChunk.getPos().x();
    }

    @Override
    public int z() {
        return levelChunk.getPos().z();
    }

    @Override
    public World world() {
        return fabricWorld;
    }

    // ============================================================
    // Chunk State
    // ============================================================

    @Override
    public boolean loaded() {
        return levelChunk.getFullStatus().isOrAfter(FullChunkStatus.FULL);
    }

    @Override
    public void load() {
        var chunkSource = fabricWorld.level().getChunkSource();
        chunkSource.getChunk(x(), z(), ChunkStatus.FULL, true);
    }

    @Override
    public void unload() {
        fabricWorld.level().unload(levelChunk);
    }

    // ============================================================
    // Biome Access & Modification
    // ============================================================

    @Override
    public Biome biomeAt(int x, int y, int z) {
        var biomeHolder = levelChunk.getNoiseBiome(x >> 2, y >> 2, z >> 2);
        var optionalKey = biomeHolder.unwrapKey();

        if (optionalKey.isEmpty()) return Biome.UNKNOWN;

        var resourceKey = optionalKey.get();
        var identifier = resourceKey.identifier();
        var biomeId = identifier.getNamespace() + ":" + identifier.getPath();
        return Biome.of(biomeId);
    }

    @Override
    public void biome(Biome biome, Vector3 position) {
        int x = (int) position.x();
        int y = (int) position.y();
        int z = (int) position.z();

        ((BiomeWritable) levelChunk).setBiome(x >> 2, y >> 2, z >> 2, biome,
                fabricWorld.level().registryAccess());

        levelChunk.markUnsaved();
    }
}
