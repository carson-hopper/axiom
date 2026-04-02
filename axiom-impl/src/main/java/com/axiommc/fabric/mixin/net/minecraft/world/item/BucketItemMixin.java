package com.axiommc.fabric.mixin.net.minecraft.world.item;

import com.axiommc.fabric.event.adapter.PlayerWorldInteractAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.InteractionResult;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.BucketItem;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.material.Fluid;
import net.minecraft.world.level.material.Fluids;
import net.minecraft.world.phys.BlockHitResult;
import net.minecraft.world.phys.HitResult;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts bucket usage to fire PlayerBucketEvent.Empty and .Fill.
 */
@Mixin(value = BucketItem.class, remap = false)
public abstract class BucketItemMixin {

    @Shadow
    @Final
    private Fluid content;

    @Inject(method = "use", at = @At("HEAD"), cancellable = true)
    private void onUse(
        Level level,
        Player player,
        InteractionHand hand,
        CallbackInfoReturnable<InteractionResult> callbackInfo) {
        if (!(player instanceof ServerPlayer serverPlayer)) {
            return;
        }
        if (!(level instanceof ServerLevel serverLevel)) {
            return;
        }
        ItemStack bucket = player.getItemInHand(hand);
        // Use the player's current pick result to find the target block
        HitResult hitResult = player.pick(5.0, 0.0f, content == Fluids.EMPTY);
        if (hitResult.getType() != HitResult.Type.BLOCK) {
            return;
        }
        BlockPos pos = ((BlockHitResult) hitResult).getBlockPos();
        boolean cancelled;
        if (content == Fluids.EMPTY) {
            cancelled =
                PlayerWorldInteractAdapter.onBucketFill(serverPlayer, bucket, pos, serverLevel);
        } else {
            cancelled =
                PlayerWorldInteractAdapter.onBucketEmpty(serverPlayer, bucket, pos, serverLevel);
        }
        if (cancelled) {
            callbackInfo.setReturnValue(InteractionResult.FAIL);
        }
    }
}
