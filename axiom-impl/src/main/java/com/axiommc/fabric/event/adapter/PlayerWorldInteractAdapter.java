package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerBucketEvent;
import com.axiommc.api.event.player.PlayerEggThrowEvent;
import com.axiommc.api.event.player.PlayerFishEvent;
import com.axiommc.api.event.player.PlayerHarvestEvent;
import com.axiommc.api.event.player.PlayerPortalEvent;
import com.axiommc.api.event.player.PlayerShearEvent;
import com.axiommc.api.item.Item;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.entity.FabricEntity;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.entity.Entity;

/**
 * Fires player world-interaction events: bucket use, fishing, shearing,
 * egg throwing, harvesting, and portal entry.
 */
public class PlayerWorldInteractAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerWorldInteractAdapter.eventBus = eventBus;
        PlayerWorldInteractAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when a player empties a bucket.
     *
     * @param serverPlayer the player emptying the bucket
     * @param bucket       the bucket item stack
     * @param pos          the target block position
     * @param level        the server level
     * @return true if the event was cancelled
     */
    public static boolean onBucketEmpty(
        ServerPlayer serverPlayer,
        net.minecraft.world.item.ItemStack bucket,
        BlockPos pos,
        ServerLevel level) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(bucket);
            Location blockLocation = toLocation(pos, level);
            PlayerBucketEvent.Empty event =
                new PlayerBucketEvent.Empty(player, itemStack, blockLocation);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerBucketEvent.Empty", exception);
        }
        return false;
    }

    /**
     * Called when a player fills a bucket.
     *
     * @param serverPlayer the player filling the bucket
     * @param bucket       the bucket item stack
     * @param pos          the source block position
     * @param level        the server level
     * @return true if the event was cancelled
     */
    public static boolean onBucketFill(
        ServerPlayer serverPlayer,
        net.minecraft.world.item.ItemStack bucket,
        BlockPos pos,
        ServerLevel level) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(bucket);
            Location blockLocation = toLocation(pos, level);
            PlayerBucketEvent.Fill event =
                new PlayerBucketEvent.Fill(player, itemStack, blockLocation);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerBucketEvent.Fill", exception);
        }
        return false;
    }

    /**
     * Called when a player's fishing hook retrieves.
     *
     * @param serverPlayer the player fishing
     * @param caught       the caught entity, or null if nothing was caught
     * @param state        the fishing hook state name
     * @return true if the event was cancelled
     */
    public static boolean onFish(ServerPlayer serverPlayer, Entity caught, String state) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            FabricEntity caughtEntity = caught != null ? new FabricEntity(caught) : null;
            PlayerFishEvent event = new PlayerFishEvent(player, caughtEntity, state);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerFishEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player shears an entity.
     *
     * @param serverPlayer the player shearing
     * @param target       the entity being sheared
     * @return true if the event was cancelled
     */
    public static boolean onShear(ServerPlayer serverPlayer, Entity target) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            FabricEntity entity = new FabricEntity(target);
            PlayerShearEvent event = new PlayerShearEvent(player, entity);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerShearEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player throws an egg.
     *
     * @param serverPlayer the player throwing the egg
     * @param hatching     whether the egg is hatching
     * @return true if the event was cancelled
     */
    public static boolean onEggThrow(ServerPlayer serverPlayer, boolean hatching) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerEggThrowEvent event = new PlayerEggThrowEvent(player, hatching);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerEggThrowEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player harvests a crop block.
     *
     * @param serverPlayer the player harvesting
     * @param pos          the block position
     * @param level        the server level
     * @return true if the event was cancelled
     */
    public static boolean onHarvest(ServerPlayer serverPlayer, BlockPos pos, ServerLevel level) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Location blockLocation = toLocation(pos, level);
            PlayerHarvestEvent event = new PlayerHarvestEvent(player, blockLocation);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerHarvestEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player enters a portal.
     *
     * @param serverPlayer the player entering the portal
     * @param from         the origin location
     * @param to           the destination location
     * @return true if the event was cancelled
     */
    public static boolean onPortal(ServerPlayer serverPlayer, Location from, Location to) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerPortalEvent event = new PlayerPortalEvent(player, from, to);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerPortalEvent", exception);
        }
        return false;
    }

    private static ItemStack toItemStack(net.minecraft.world.item.ItemStack mcStack) {
        String key =
            mcStack.getItem().builtInRegistryHolder().key().identifier().toString();
        return ItemStack.of(Item.of(key), mcStack.getCount());
    }

    private static Location toLocation(BlockPos pos, ServerLevel level) {
        World world = new FabricWorld(level);
        Vector3 position = new Vector3(pos.getX(), pos.getY(), pos.getZ());
        return new Location(world, position, new Vector2(0, 0));
    }
}
