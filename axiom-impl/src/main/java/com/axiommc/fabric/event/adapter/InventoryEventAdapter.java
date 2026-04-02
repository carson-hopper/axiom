package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.inventory.InventoryEvent;
import com.axiommc.api.item.Item;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.level.Level;

/**
 * Fires {@link InventoryEvent} subtypes from mixin hooks on crafting,
 * smelting, brewing, anvil, and smithing interactions.
 */
public class InventoryEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        InventoryEventAdapter.eventBus = eventBus;
    }

    // ── Static mixin hooks ──────────────────────────────────────────────

    /** Called when a player takes the result from a crafting table. */
    public static void onCraft(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack result) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(result);
            eventBus.publish(new InventoryEvent.Craft(player, itemStack));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.Craft", exception);
        }
    }

    /** Called when a craft result is previewed. */
    public static void onCraftPrepare(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack result) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(result);
            eventBus.publish(new InventoryEvent.CraftPrepare(player, itemStack));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.CraftPrepare", exception);
        }
    }

    /**
     * Called when a furnace completes smelting.
     *
     * @return true if the event was cancelled
     */
    public static boolean onSmelt(
        ServerLevel serverLevel,
        BlockPos pos,
        net.minecraft.world.item.ItemStack source,
        net.minecraft.world.item.ItemStack result) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricWorld world = new FabricWorld(serverLevel);
            Location location = toLocation(world, pos);
            InventoryEvent.Smelt event =
                new InventoryEvent.Smelt(world, location, toItemStack(source), toItemStack(result));
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.Smelt", exception);
            return false;
        }
    }

    /**
     * Called when fuel is consumed in a furnace.
     *
     * @return true if the event was cancelled
     */
    public static boolean onFurnaceBurn(
        ServerLevel serverLevel, BlockPos pos, net.minecraft.world.item.ItemStack fuel) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricWorld world = new FabricWorld(serverLevel);
            Location location = toLocation(world, pos);
            InventoryEvent.FurnaceBurn event =
                new InventoryEvent.FurnaceBurn(world, location, toItemStack(fuel));
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.FurnaceBurn", exception);
            return false;
        }
    }

    /** Called when a player takes items from a furnace output slot. */
    public static void onFurnaceExtract(
        ServerPlayer serverPlayer,
        BlockPos pos,
        net.minecraft.world.item.ItemStack item,
        int amount) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ServerLevel serverLevel = (ServerLevel) serverPlayer.level();
            FabricWorld world = new FabricWorld(serverLevel);
            Location location = toLocation(world, pos);
            eventBus.publish(
                new InventoryEvent.FurnaceExtract(player, location, toItemStack(item), amount));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.FurnaceExtract", exception);
        }
    }

    /**
     * Called when a brewing stand completes brewing.
     *
     * @return true if the event was cancelled
     */
    public static boolean onBrew(Level level, BlockPos pos) {
        if (eventBus == null) {
            return false;
        }
        try {
            ServerLevel serverLevel = (ServerLevel) level;
            FabricWorld world = new FabricWorld(serverLevel);
            Location location = toLocation(world, pos);
            InventoryEvent.Brew event = new InventoryEvent.Brew(world, location);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.Brew", exception);
            return false;
        }
    }

    /** Called when an anvil result is previewed or taken. */
    public static void onAnvilPrepare(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack result) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(result);
            eventBus.publish(new InventoryEvent.AnvilPrepare(player, itemStack));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.AnvilPrepare", exception);
        }
    }

    /** Called when a player takes the result from a smithing table. */
    public static void onSmith(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack result) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(result);
            eventBus.publish(new InventoryEvent.Smith(player, itemStack));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.Smith", exception);
        }
    }

    /**
     * Called when a player enchants an item.
     *
     * @return true if the event was cancelled
     */
    public static boolean onEnchant(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack item, int level) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            InventoryEvent.Enchant event =
                new InventoryEvent.Enchant(player, toItemStack(item), level);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.Enchant", exception);
            return false;
        }
    }

    /** Called when a player places an item in an enchantment table. */
    public static void onEnchantPrepare(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack item) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new InventoryEvent.EnchantPrepare(player, toItemStack(item)));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing InventoryEvent.EnchantPrepare", exception);
        }
    }

    // ── Helpers ─────────────────────────────────────────────────────────

    private static ItemStack toItemStack(net.minecraft.world.item.ItemStack mcStack) {
        if (mcStack == null || mcStack.isEmpty()) {
            return ItemStack.of(Item.of("minecraft:air"), 0);
        }
        String key =
            mcStack.getItem().builtInRegistryHolder().key().identifier().toString();
        return ItemStack.of(Item.of(key), mcStack.getCount());
    }

    private static Location toLocation(FabricWorld world, BlockPos pos) {
        return new Location(
            world, new Vector3(pos.getX(), pos.getY(), pos.getZ()), new Vector2(0, 0));
    }
}
