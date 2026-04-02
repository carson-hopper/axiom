package com.axiommc.fabric.mixin.net.minecraft.world.entity;

import com.axiommc.fabric.event.adapter.EntityEventAdapter;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.LivingEntity;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts living entity death to fire EntityEvent.Death.
 */
@Mixin(value = LivingEntity.class, remap = false)
public abstract class LivingEntityMixin {

    @Inject(method = "die", at = @At("HEAD"))
    private void onDie(DamageSource damageSource, CallbackInfo callbackInfo) {
        EntityEventAdapter.onDeath((LivingEntity) (Object) this);
    }
}
