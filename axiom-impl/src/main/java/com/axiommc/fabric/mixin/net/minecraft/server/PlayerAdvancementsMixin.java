package com.axiommc.fabric.mixin.net.minecraft.server;

import com.axiommc.fabric.event.adapter.PlayerActionAdapter;
import net.minecraft.advancements.AdvancementHolder;
import net.minecraft.server.PlayerAdvancements;
import net.minecraft.server.level.ServerPlayer;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts advancement awards on {@link PlayerAdvancements} to fire
 * {@link com.axiommc.api.event.player.PlayerAdvancementEvent}.
 */
@Mixin(value = PlayerAdvancements.class, remap = false)
public abstract class PlayerAdvancementsMixin {

    @Shadow
    private ServerPlayer player;

    @Inject(method = "award", at = @At("HEAD"))
    private void onAward(
        AdvancementHolder advancementHolder,
        String criterionKey,
        CallbackInfoReturnable<Boolean> callbackInfo) {
        String advancementId = advancementHolder.id().toString();
        PlayerActionAdapter.onAdvancement(player, advancementId);
    }
}
