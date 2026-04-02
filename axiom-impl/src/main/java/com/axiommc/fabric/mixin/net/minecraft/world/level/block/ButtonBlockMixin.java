package com.axiommc.fabric.mixin.net.minecraft.world.level.block;

import com.axiommc.fabric.event.adapter.RedstoneEventAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.ButtonBlock;
import net.minecraft.world.level.block.state.BlockState;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts button presses to fire RedstoneEvent.ButtonPress.
 */
@Mixin(value = ButtonBlock.class, remap = false)
public abstract class ButtonBlockMixin {

    @Inject(method = "press", at = @At("HEAD"))
    private void onPress(
        BlockState state, Level level, BlockPos pos, Player player, CallbackInfo callbackInfo) {
        if (level instanceof ServerLevel serverLevel) {
            RedstoneEventAdapter.onButtonPress(serverLevel, pos);
        }
    }
}
