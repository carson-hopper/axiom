package com.axiommc.fabric.mixin.net.minecraft.world.clock;

import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import net.minecraft.core.Holder;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.clock.ServerClockManager;
import net.minecraft.world.clock.WorldClock;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts clock time changes to fire WorldTimeSkipEvent.
 */
@Mixin(value = ServerClockManager.class, remap = false)
public abstract class ServerClockManagerMixin {

    @Shadow
    private MinecraftServer server;

    @Shadow
    public abstract long getTotalTicks(Holder<WorldClock> clock);

    @Inject(method = "setTotalTicks", at = @At("HEAD"), cancellable = true)
    private void onSetTotalTicks(Holder<WorldClock> clock, long ticks, CallbackInfo callbackInfo) {
        if (server == null) {
            return;
        }
        long oldTicks = getTotalTicks(clock);
        long delta = ticks - oldTicks;
        if (delta == 0L) {
            return;
        }
        ServerLevel overworld = server.overworld();
        if (!WorldEventAdapter.onTimeSkip(overworld, delta, "command")) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "addTicks", at = @At("HEAD"), cancellable = true)
    private void onAddTicks(Holder<WorldClock> clock, int ticks, CallbackInfo callbackInfo) {
        if (server == null || ticks == 0) {
            return;
        }
        ServerLevel overworld = server.overworld();
        if (!WorldEventAdapter.onTimeSkip(overworld, ticks, "command")) {
            callbackInfo.cancel();
        }
    }
}
