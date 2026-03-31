package com.axiommc.fabric.mixin.net.minecraft.world.level.chunk;

import com.axiommc.api.world.BiomeWritable;
import net.minecraft.core.QuartPos;
import net.minecraft.util.Mth;
import net.minecraft.world.level.chunk.ChunkAccess;
import net.minecraft.world.level.chunk.LevelChunkSection;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;

@Mixin(value = ChunkAccess.class, remap = false)
public abstract class ChunkAccessMixin implements BiomeWritable {

    @Final
    @Shadow protected LevelChunkSection[] sections;

    @Final
    @Shadow protected net.minecraft.world.level.LevelHeightAccessor levelHeightAccessor;

    @Override
    public void setBiome(int x, int y, int z, com.axiommc.api.world.Biome biome, Object registryAccess) {
        int quartMinY = QuartPos.fromBlock(this.levelHeightAccessor.getMinY());
        int quartMaxY = quartMinY + QuartPos.fromBlock(this.levelHeightAccessor.getHeight()) - 1;
        int clampedY = Mth.clamp(y, quartMinY, quartMaxY);
        int sectionIndex = this.levelHeightAccessor.getSectionIndex(QuartPos.toBlock(clampedY));
        ((BiomeWritable) this.sections[sectionIndex]).setBiome(x & 3, clampedY & 3, z & 3, biome, registryAccess);
    }
}