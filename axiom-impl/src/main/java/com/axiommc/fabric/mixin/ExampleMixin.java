package com.axiommc.fabric.mixin;

import net.minecraft.server.MinecraftServer;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

@Mixin(value = MinecraftServer.class, remap = false)
public class ExampleMixin {

    @Inject(at = @At("HEAD"), method = "loadLevel", remap = false)
    private void onLoadLevel(CallbackInfo info) {
        System.out.println("Axiom mixin fired on loadLevel!");
    }
}