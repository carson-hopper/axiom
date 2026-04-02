package com.axiommc.api.event.inventory;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.World;

/**
 * Events fired for inventory-related actions.
 *
 * <ul>
 *   <li>{@link Craft} — player crafts an item
 *   <li>{@link CraftPrepare} — craft result is previewed
 *   <li>{@link Smelt} — furnace completes smelting; cancellable
 *   <li>{@link FurnaceBurn} — fuel is consumed in a furnace; cancellable
 *   <li>{@link FurnaceExtract} — player takes items from a furnace
 *   <li>{@link Brew} — brewing stand completes brewing; cancellable
 *   <li>{@link AnvilPrepare} — anvil result is previewed
 *   <li>{@link Smith} — player uses a smithing table
 *   <li>{@link Enchant} — item enchanted at an enchantment table; cancellable
 *   <li>{@link EnchantPrepare} — item placed in an enchantment table
 * </ul>
 */
public final class InventoryEvent {

    private InventoryEvent() {}

    /** Fired when a player crafts an item. */
    public static class Craft extends Event {

        private final Player player;
        private final ItemStack result;

        public Craft(Player player, ItemStack result) {
            this.player = player;
            this.result = result;
        }

        public Player player() {
            return player;
        }

        public ItemStack result() {
            return result;
        }
    }

    /** Fired when a craft result is previewed. */
    public static class CraftPrepare extends Event {

        private final Player player;
        private final ItemStack result;

        public CraftPrepare(Player player, ItemStack result) {
            this.player = player;
            this.result = result;
        }

        public Player player() {
            return player;
        }

        public ItemStack result() {
            return result;
        }
    }

    /** Fired when a furnace completes smelting. */
    public static class Smelt extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final ItemStack source;
        private final ItemStack result;
        private boolean cancelled = false;

        public Smelt(World world, Location blockLocation, ItemStack source, ItemStack result) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.source = source;
            this.result = result;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public ItemStack source() {
            return source;
        }

        public ItemStack result() {
            return result;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /** Fired when fuel is consumed in a furnace. */
    public static class FurnaceBurn extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final ItemStack fuel;
        private boolean cancelled = false;

        public FurnaceBurn(World world, Location blockLocation, ItemStack fuel) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.fuel = fuel;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public ItemStack fuel() {
            return fuel;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /** Fired when a player takes items from a furnace. */
    public static class FurnaceExtract extends Event {

        private final Player player;
        private final Location blockLocation;
        private final ItemStack item;
        private final int amount;

        public FurnaceExtract(Player player, Location blockLocation, ItemStack item, int amount) {
            this.player = player;
            this.blockLocation = blockLocation;
            this.item = item;
            this.amount = amount;
        }

        public Player player() {
            return player;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public ItemStack item() {
            return item;
        }

        public int amount() {
            return amount;
        }
    }

    /** Fired when a brewing stand completes brewing. */
    public static class Brew extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private boolean cancelled = false;

        public Brew(World world, Location blockLocation) {
            this.world = world;
            this.blockLocation = blockLocation;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /** Fired when an anvil result is previewed. */
    public static class AnvilPrepare extends Event {

        private final Player player;
        private final ItemStack result;

        public AnvilPrepare(Player player, ItemStack result) {
            this.player = player;
            this.result = result;
        }

        public Player player() {
            return player;
        }

        public ItemStack result() {
            return result;
        }
    }

    /** Fired when a player uses a smithing table. */
    public static class Smith extends Event {

        private final Player player;
        private final ItemStack result;

        public Smith(Player player, ItemStack result) {
            this.player = player;
            this.result = result;
        }

        public Player player() {
            return player;
        }

        public ItemStack result() {
            return result;
        }
    }

    /** Fired when an item is enchanted at an enchantment table. */
    public static class Enchant extends Event implements Cancellable {

        private final Player player;
        private final ItemStack item;
        private final int level;
        private boolean cancelled = false;

        public Enchant(Player player, ItemStack item, int level) {
            this.player = player;
            this.item = item;
            this.level = level;
        }

        public Player player() {
            return player;
        }

        public ItemStack item() {
            return item;
        }

        public int level() {
            return level;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /** Fired when an item is placed in an enchantment table. */
    public static class EnchantPrepare extends Event {

        private final Player player;
        private final ItemStack item;

        public EnchantPrepare(Player player, ItemStack item) {
            this.player = player;
            this.item = item;
        }

        public Player player() {
            return player;
        }

        public ItemStack item() {
            return item;
        }
    }
}
