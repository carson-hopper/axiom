package com.axiommc.fabric.mixin.net.minecraft.server.level;

import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.LightningBolt;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts entity spawning on ServerLevel to fire lightning events.
 */
@Mixin(value = ServerLevel.class, remap = false)
public abstract class ServerLevelMixin {

    @Inject(method = "addFreshEntity", at = @At("HEAD"), cancellable = true)
    private void onAddFreshEntity(Entity entity, CallbackInfoReturnable<Boolean> callbackInfo) {
        if (entity instanceof LightningBolt) {
            ServerLevel self = (ServerLevel) (Object) this;
            if (!WorldEventAdapter.onLightningStrike(
                self, entity.getX(), entity.getY(), entity.getZ())) {
                callbackInfo.setReturnValue(false);
            }
        }
    }
}
