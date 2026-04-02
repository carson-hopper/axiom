package com.axiommc.fabric.mixin.net.minecraft.server.level;

import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import net.minecraft.server.level.ChunkMap;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.ChunkPos;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts chunk loading and unloading to fire WorldChunkEvent.PreLoad
 * and WorldChunkEvent.PreUnload.
 */
@Mixin(value = ChunkMap.class, remap = false)
public abstract class ChunkMapMixin {

    @Shadow
    @Final
    ServerLevel level;

    @Inject(method = "scheduleChunkLoad", at = @At("HEAD"), cancellable = true)
    private void onScheduleChunkLoad(ChunkPos chunkPos, CallbackInfoReturnable<?> callbackInfo) {
        if (!WorldEventAdapter.onPreChunkLoad(level, chunkPos.x(), chunkPos.z())) {
            callbackInfo.setReturnValue(null);
        }
    }

    @Inject(method = "scheduleUnload", at = @At("HEAD"), cancellable = true)
    private void onScheduleUnload(
        long chunkPosLong,
        net.minecraft.server.level.ChunkHolder holder,
        CallbackInfo callbackInfo) {
        int chunkX = ChunkPos.getX(chunkPosLong);
        int chunkZ = ChunkPos.getZ(chunkPosLong);
        ChunkPos chunkPos = new ChunkPos(chunkX, chunkZ);
        if (!WorldEventAdapter.onPreChunkUnload(level, chunkPos.x(), chunkPos.z())) {
            callbackInfo.cancel();
        }
    }
}
