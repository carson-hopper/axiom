package com.axiommc.fabric.mixin.net.minecraft.world.level.saveddata;

import com.axiommc.fabric.AxiomMod;
import com.axiommc.fabric.event.adapter.WorldEventAdapter;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.saveddata.WeatherData;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts weather state changes to fire WorldWeatherEvent.
 */
@Mixin(value = WeatherData.class, remap = false)
public abstract class WeatherDataMixin {

    @Shadow
    private boolean raining;

    @Shadow
    private boolean thundering;

    @Inject(method = "setRaining", at = @At("HEAD"), cancellable = true)
    private void onSetRaining(boolean newRaining, CallbackInfo callbackInfo) {
        if (newRaining != raining) {
            ServerLevel overworld = overworld();
            if (overworld != null && !WorldEventAdapter.onWeatherChange(overworld, newRaining)) {
                callbackInfo.cancel();
            }
        }
    }

    @Inject(method = "setThundering", at = @At("HEAD"), cancellable = true)
    private void onSetThundering(boolean newThundering, CallbackInfo callbackInfo) {
        if (newThundering != thundering) {
            ServerLevel overworld = overworld();
            if (overworld != null && !WorldEventAdapter.onThunderChange(overworld, newThundering)) {
                callbackInfo.cancel();
            }
        }
    }

    private static ServerLevel overworld() {
        MinecraftServer server =
            AxiomMod.instance() != null ? AxiomMod.instance().minecraftServer() : null;
        return server != null ? server.overworld() : null;
    }
}
