package com.axiommc.fabric.mixin.net.minecraft.world.entity.player;

import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.player.Player;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.gen.Invoker;

@Mixin(value = Player.class, remap = false)
public interface PlayerAccessor {

    @Invoker("actuallyHurt")
    void invokeActuallyHurt(ServerLevel level, DamageSource source, float amount);
}
