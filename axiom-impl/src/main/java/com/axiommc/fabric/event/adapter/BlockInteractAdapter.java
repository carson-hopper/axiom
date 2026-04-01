package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.block.BlockInteractEvent;
import com.axiommc.fabric.block.FabricBlock;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import net.fabricmc.fabric.api.event.player.UseBlockCallback;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.InteractionResult;
import com.axiommc.fabric.Axiom;

/**
 * Fires BlockInteractEvent.MainHand and BlockInteractEvent.OffHand
 * using Fabric's UseBlockCallback.
 */
public class BlockInteractAdapter implements FabricEventAdapter {

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        UseBlockCallback.EVENT.register((player, world, hand, hitResult) -> {
            if (!(player instanceof ServerPlayer serverPlayer)) {
                return InteractionResult.PASS;
            }

            if (!(world instanceof ServerLevel serverLevel)) {
                return InteractionResult.PASS;
            }

            try {
                var axiomPlayer = playerProvider.player(serverPlayer.getUUID());
                if (axiomPlayer.isEmpty()) {
                    return InteractionResult.PASS;
                }

                BlockPos pos = hitResult.getBlockPos();
                var fabricWorld = new FabricWorld(serverLevel);
                var block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), fabricWorld);

                if (hand == InteractionHand.MAIN_HAND) {
                    var event = new BlockInteractEvent.MainHand(axiomPlayer.get(), block);
                    eventBus.publish(event);
                    if (event.isCancelled()) {
                        return InteractionResult.FAIL;
                    }
                } else {
                    var event = new BlockInteractEvent.OffHand(axiomPlayer.get(), block);
                    eventBus.publish(event);
                    if (event.isCancelled()) {
                        return InteractionResult.FAIL;
                    }
                }
            } catch (Exception e) {
                Axiom.logger().debug("Error firing BlockInteractEvent", e);
            }

            return InteractionResult.PASS;
        });
    }
}
