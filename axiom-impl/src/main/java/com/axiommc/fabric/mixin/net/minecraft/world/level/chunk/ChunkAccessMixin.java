package com.axiommc.fabric.mixin.net.minecraft.world.level.chunk;

import net.minecraft.world.level.chunk.ChunkAccess;
import org.spongepowered.asm.mixin.Mixin;

@Mixin(value = ChunkAccess.class, remap = false)
public abstract class ChunkAccessMixin {

//    @Final
//    @Shadow protected LevelChunkSection[] sections;
//
//    @Final
//    @Shadow protected net.minecraft.world.level.LevelHeightAccessor levelHeightAccessor;
//
//    @Override
//    public void setBiome(int x, int y, int z, com.axiommc.api.world.Biome biome, Object registryAccess) {
//        int quartMinY = QuartPos.fromBlock(this.levelHeightAccessor.getMinY());
//        int quartMaxY = quartMinY + QuartPos.fromBlock(this.levelHeightAccessor.getHeight()) - 1;
//        int clampedY = Mth.clamp(y, quartMinY, quartMaxY);
//        int sectionIndex = this.levelHeightAccessor.getSectionIndex(QuartPos.toBlock(clampedY));
//
//        // Set biome directly on the LevelChunkSection's biomes palette
//        LevelChunkSection section = this.sections[sectionIndex];
//        net.minecraft.core.RegistryAccess access = (net.minecraft.core.RegistryAccess) registryAccess;
//        net.minecraft.core.Registry<net.minecraft.world.level.biome.Biome> biomeRegistry =
//                access.lookupOrThrow(Registries.BIOME);
//        Identifier location = Identifier.parse(biome.id());
//        biomeRegistry.get(location).ifPresent(h -> {
//            PalettedContainerRO<Holder<net.minecraft.world.level.biome.Biome>> biomes = section.getBiomes();
//            if (biomes instanceof PalettedContainer<?>) {
//                ((PalettedContainer<Holder<net.minecraft.world.level.biome.Biome>>) biomes).set(x & 3, clampedY & 3, z & 3, h);
//            }
//        });
//    }
}