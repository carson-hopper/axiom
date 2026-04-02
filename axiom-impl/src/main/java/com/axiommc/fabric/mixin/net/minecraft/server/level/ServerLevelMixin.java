package com.axiommc.fabric.mixin.net.minecraft.server.level;

import com.axiommc.fabric.event.adapter.EntityEventAdapter;
import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import java.util.function.BooleanSupplier;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.LightningBolt;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts entity spawning on ServerLevel to fire spawn, lightning, and sleep time-skip events.
 */
@Mixin(value = ServerLevel.class, remap = false)
public abstract class ServerLevelMixin {

    @Inject(method = "addFreshEntity", at = @At("HEAD"), cancellable = true)
    private void onAddFreshEntity(Entity entity, CallbackInfoReturnable<Boolean> callbackInfo) {
        ServerLevel self = (ServerLevel) (Object) this;

        // Fire EntityEvent.Spawn for all entities
        if (!EntityEventAdapter.onSpawn(entity, self)) {
            callbackInfo.setReturnValue(false);
            return;
        }

        // Fire WorldWeatherEvent.LightningStrike for lightning bolts
        if (entity instanceof LightningBolt) {
            if (!WorldEventAdapter.onLightningStrike(
                self, entity.getX(), entity.getY(), entity.getZ())) {
                callbackInfo.setReturnValue(false);
            }
        }
    }

    /**
     * Fires WorldTimeSkipEvent with reason "sleep" before players are woken up.
     * The wakeUpAllPlayers call in tick() indicates a sleep-based time advancement.
     */
    @Inject(
        method = "tick",
        at =
            @At(
                value = "INVOKE",
                target = "Lnet/minecraft/server/level/ServerLevel;wakeUpAllPlayers()V"),
        cancellable = true)
    private void onSleepTimeSkip(BooleanSupplier hasTimeLeft, CallbackInfo callbackInfo) {
        ServerLevel self = (ServerLevel) (Object) this;
        // Fire the event with delta=0 since the actual time change happens via the clock manager.
        // Plugins can use this to detect sleep-based skips and cancel them.
        WorldEventAdapter.onTimeSkip(self, 0L, "sleep");
    }
}
