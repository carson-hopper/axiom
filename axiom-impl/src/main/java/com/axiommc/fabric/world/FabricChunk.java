package com.axiommc.fabric.world;

import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.Biome;
import com.axiommc.api.world.BiomeWritable;
import com.axiommc.api.world.Chunk;
import com.axiommc.api.world.World;
import net.minecraft.core.Holder;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.FullChunkStatus;
import net.minecraft.world.level.chunk.ChunkSource;
import net.minecraft.world.level.chunk.LevelChunk;
import net.minecraft.world.level.chunk.status.ChunkStatus;

import java.util.Optional;

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
        ChunkSource chunkSource = fabricWorld.level().getChunkSource();
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
        Holder<net.minecraft.world.level.biome.Biome> biomeHolder = levelChunk.getNoiseBiome(x >> 2, y >> 2, z >> 2);

        try {
            // Try to get biome ID from the holder's key
            Optional<?> optionalKey = biomeHolder.unwrapKey();
            if (optionalKey.isPresent()) {
                // Get the resource key
                Object key = optionalKey.get();
                // Try to get the location from the key using reflection
                try {
                    java.lang.reflect.Method locationMethod = key.getClass().getMethod("location");
                    Identifier identifier = (Identifier) locationMethod.invoke(key);
                    String biomeIdStr = identifier.getNamespace() + ":" + identifier.getPath();
                    return Biome.of(biomeIdStr);
                } catch (Exception e2) {
                    // If reflection fails, try toString()
                    String biomeStr = key.toString();
                    if (biomeStr.contains(":")) {
                        return Biome.of(biomeStr.replaceAll("[^\\w:]*", ""));
                    }
                }
            }
        } catch (Exception e) {
            // Fall through to default
        }

        return Biome.UNKNOWN;
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
