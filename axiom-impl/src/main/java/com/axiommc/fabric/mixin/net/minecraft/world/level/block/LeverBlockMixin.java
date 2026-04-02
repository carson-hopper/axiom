package com.axiommc.fabric.mixin.net.minecraft.world.level.block;

import com.axiommc.fabric.event.adapter.RedstoneEventAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.LeverBlock;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.block.state.properties.BlockStateProperties;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts lever toggles to fire RedstoneEvent.LeverToggle.
 */
@Mixin(value = LeverBlock.class, remap = false)
public abstract class LeverBlockMixin {

    @Inject(method = "pull", at = @At("HEAD"))
    private void onPull(
        BlockState state, Level level, BlockPos pos, Player player, CallbackInfo callbackInfo) {
        if (level instanceof ServerLevel serverLevel) {
            boolean currentlyPowered = state.getValue(BlockStateProperties.POWERED);
            RedstoneEventAdapter.onLeverToggle(serverLevel, pos, !currentlyPowered);
        }
    }
}
