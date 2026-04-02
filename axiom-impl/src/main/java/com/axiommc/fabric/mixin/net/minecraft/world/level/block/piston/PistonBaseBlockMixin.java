package com.axiommc.fabric.mixin.net.minecraft.world.level.block.piston;

import com.axiommc.fabric.event.adapter.RedstoneEventAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.piston.PistonBaseBlock;
import net.minecraft.world.level.block.state.BlockState;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts piston extend/retract to fire RedstoneEvent.PistonExtend and PistonRetract.
 */
@Mixin(value = PistonBaseBlock.class, remap = false)
public abstract class PistonBaseBlockMixin {

    @Inject(method = "triggerEvent", at = @At("HEAD"), cancellable = true)
    private void onTriggerEvent(
        BlockState state,
        Level level,
        BlockPos pos,
        int type,
        int data,
        CallbackInfoReturnable<Boolean> callbackInfo) {
        if (!(level instanceof ServerLevel serverLevel)) {
            return;
        }
        boolean allowed;
        if (type == 0) {
            allowed = RedstoneEventAdapter.onPistonExtend(serverLevel, pos);
        } else if (type == 1) {
            allowed = RedstoneEventAdapter.onPistonRetract(serverLevel, pos);
        } else {
            return;
        }
        if (!allowed) {
            callbackInfo.setReturnValue(false);
        }
    }
}
