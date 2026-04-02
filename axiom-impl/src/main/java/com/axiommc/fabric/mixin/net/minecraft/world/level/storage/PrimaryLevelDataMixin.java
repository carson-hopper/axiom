package com.axiommc.fabric.mixin.net.minecraft.world.level.storage;

import com.axiommc.api.math.Vector3;
import com.axiommc.fabric.event.adapter.ServerLifecycleAdapter;
import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.storage.LevelData;
import net.minecraft.world.level.storage.PrimaryLevelData;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts world spawn point changes to fire WorldSpawnChangeEvent.
 */
@Mixin(value = PrimaryLevelData.class, remap = false)
public abstract class PrimaryLevelDataMixin {

    @Shadow
    private LevelData.RespawnData respawnData;

    @Inject(method = "setSpawn", at = @At("HEAD"))
    private void onSetSpawn(LevelData.RespawnData newRespawnData, CallbackInfo callbackInfo) {
        MinecraftServer server = ServerLifecycleAdapter.minecraftServer();
        if (server == null) {
            return;
        }

        ServerLevel overworld = server.overworld();
        if (overworld == null) {
            return;
        }

        // Only fire if this is the overworld's level data
        if (overworld.getLevelData() != (Object) this) {
            // Check other dimensions
            for (ServerLevel level : server.getAllLevels()) {
                if (level.getLevelData() == (Object) this) {
                    fireSpawnChangeEvent(level, newRespawnData);
                    return;
                }
            }
            return;
        }

        fireSpawnChangeEvent(overworld, newRespawnData);
    }

    private void fireSpawnChangeEvent(ServerLevel level, LevelData.RespawnData newRespawnData) {
        Vector3 oldSpawn;
        if (respawnData != null) {
            BlockPos oldPos = respawnData.pos();
            oldSpawn = new Vector3(oldPos.getX(), oldPos.getY(), oldPos.getZ());
        } else {
            oldSpawn = Vector3.ZERO;
        }

        BlockPos newPos = newRespawnData.pos();
        Vector3 newSpawn = new Vector3(newPos.getX(), newPos.getY(), newPos.getZ());

        WorldEventAdapter.onSpawnChange(level, oldSpawn, newSpawn);
    }
}
