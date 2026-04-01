package com.axiommc.fabric.mixin.net.minecraft.world.level.chunk;

import com.axiommc.api.world.BiomeWritable;
import net.minecraft.core.Holder;
import net.minecraft.core.Registry;
import net.minecraft.core.RegistryAccess;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.Identifier;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.chunk.LevelChunkSection;
import net.minecraft.world.level.chunk.PalettedContainer;
import net.minecraft.world.level.chunk.PalettedContainerRO;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;

@Mixin(value = LevelChunkSection.class, remap = false)
public class LevelChunkSectionMixin implements BiomeWritable {

    @Shadow private PalettedContainerRO<Holder<net.minecraft.world.level.biome.Biome>> biomes;

    @Override
    public void setBiome(int x, int y, int z, com.axiommc.api.world.Biome biome, Object registryAccess) {
        RegistryAccess access = (RegistryAccess) registryAccess;
        Registry<Biome> biomeRegistry =
                access.lookupOrThrow(Registries.BIOME);
        Identifier location = Identifier.parse(biome.id());
        biomeRegistry.get(location)
                .ifPresent(h -> ((PalettedContainer<Holder<net.minecraft.world.level.biome.Biome>>) this.biomes)
                        .set(x, y, z, h));
    }
}