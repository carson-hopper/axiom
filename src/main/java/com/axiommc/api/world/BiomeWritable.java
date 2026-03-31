package com.axiommc.api.world;

public interface BiomeWritable {

    void setBiome(int x, int y, int z, Biome biome, Object registryAccess);

}
