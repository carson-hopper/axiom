package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.block.BlockEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.block.FabricBlock;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import java.util.Optional;
import net.fabricmc.fabric.api.event.player.PlayerBlockBreakEvents;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link BlockEvent.Break} via Fabric's {@link PlayerBlockBreakEvents} callback
 * and provides a static hook for {@link BlockEvent.Place} (called from a mixin).
 */
public class BlockEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        BlockEventAdapter.eventBus = eventBus;
        BlockEventAdapter.playerProvider = playerProvider;

        PlayerBlockBreakEvents.BEFORE.register((world, player, pos, state, blockEntity) -> {
            if (!(player instanceof ServerPlayer serverPlayer)) {
                return true;
            }

            if (!(world instanceof ServerLevel serverLevel)) {
                return true;
            }

            try {
                Optional<FabricPlayer> axiomPlayer = playerProvider.player(serverPlayer.getUUID());
                if (axiomPlayer.isEmpty()) {
                    return true;
                }

                FabricWorld fabricWorld = new FabricWorld(serverLevel);
                FabricBlock block =
                    new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), fabricWorld);

                BlockEvent.Break event = new BlockEvent.Break(axiomPlayer.get(), block);
                eventBus.publish(event);
                return !event.isCancelled();
            } catch (Exception e) {
                Axiom.logger().debug("Error firing BlockEvent.Break", e);
            }

            return true;
        });
    }

    /**
     * Called from a mixin to fire {@link BlockEvent.Place}.
     *
     * @param serverPlayer the player who placed the block
     * @param pos          the position of the placed block
     * @return true if the event was cancelled
     */
    public static boolean onPlace(ServerPlayer serverPlayer, BlockPos pos) {
        if (eventBus == null || playerProvider == null) {
            return false;
        }

        try {
            Optional<FabricPlayer> axiomPlayer = playerProvider.player(serverPlayer.getUUID());
            if (axiomPlayer.isEmpty()) {
                return false;
            }

            ServerLevel serverLevel = (ServerLevel) serverPlayer.level();
            FabricWorld fabricWorld = new FabricWorld(serverLevel);
            FabricBlock block =
                new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), fabricWorld);

            BlockEvent.Place event = new BlockEvent.Place(axiomPlayer.get(), block);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception e) {
            Axiom.logger().debug("Error firing BlockEvent.Place", e);
        }

        return false;
    }
}
