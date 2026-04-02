package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player interacts with items.
 *
 * <ul>
 *   <li>{@link Drop} — player drops an item stack; cancellable
 *   <li>{@link Pickup} — player picks up an item stack; cancellable
 *   <li>{@link Break} — player's item breaks from durability loss
 *   <li>{@link Consume} — player consumes an item; cancellable
 *   <li>{@link Durability} — player's item loses durability; cancellable
 * </ul>
 */
public final class PlayerItemEvent {

    private PlayerItemEvent() {}

    /** Fired when a player drops an item stack. */
    public static class Drop extends Event implements Cancellable {

        private final Player player;
        private final ItemStack itemStack;
        private boolean cancelled = false;

        public Drop(Player player, ItemStack itemStack) {
            this.player = player;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public ItemStack itemStack() {
            return itemStack;
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

    /** Fired when a player picks up an item stack. */
    public static class Pickup extends Event implements Cancellable {

        private final Player player;
        private final ItemStack itemStack;
        private boolean cancelled = false;

        public Pickup(Player player, ItemStack itemStack) {
            this.player = player;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public ItemStack itemStack() {
            return itemStack;
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

    /** Fired when a player's item breaks from durability loss. */
    public static class Break extends Event {

        private final Player player;
        private final ItemStack itemStack;

        public Break(Player player, ItemStack itemStack) {
            this.player = player;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public ItemStack itemStack() {
            return itemStack;
        }
    }

    /** Fired when a player consumes an item (eating or drinking). */
    public static class Consume extends Event implements Cancellable {

        private final Player player;
        private final ItemStack itemStack;
        private boolean cancelled = false;

        public Consume(Player player, ItemStack itemStack) {
            this.player = player;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public ItemStack itemStack() {
            return itemStack;
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

    /** Fired when a player's item loses durability. */
    public static class Durability extends Event implements Cancellable {

        private final Player player;
        private final ItemStack itemStack;
        private final int damage;
        private boolean cancelled = false;

        public Durability(Player player, ItemStack itemStack, int damage) {
            this.player = player;
            this.itemStack = itemStack;
            this.damage = damage;
        }

        public Player player() {
            return player;
        }

        public ItemStack itemStack() {
            return itemStack;
        }

        public int damage() {
            return damage;
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
}
