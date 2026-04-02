package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.block.BlockInteractEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.block.FabricBlock;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import java.util.Optional;
import net.fabricmc.fabric.api.event.player.UseBlockCallback;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.InteractionResult;

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
                Optional<FabricPlayer> axiomPlayer = playerProvider.player(serverPlayer.getUUID());
                if (axiomPlayer.isEmpty()) {
                    return InteractionResult.PASS;
                }

                BlockPos blockPos = hitResult.getBlockPos();
                FabricWorld fabricWorld = new FabricWorld(serverLevel);
                FabricBlock block = new FabricBlock(
                    serverLevel, blockPos.getX(), blockPos.getY(), blockPos.getZ(), fabricWorld);

                if (hand == InteractionHand.MAIN_HAND) {
                    BlockInteractEvent.MainHand event =
                        new BlockInteractEvent.MainHand(axiomPlayer.get(), block);
                    eventBus.publish(event);
                    if (event.isCancelled()) {
                        return InteractionResult.FAIL;
                    }
                } else {
                    BlockInteractEvent.OffHand event =
                        new BlockInteractEvent.OffHand(axiomPlayer.get(), block);
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
