package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player interacts with an inventory.
 *
 * <ul>
 *   <li>{@link Click} — player clicks a slot in an inventory; cancellable
 *   <li>{@link Close} — player closes an inventory
 *   <li>{@link Open} — player opens an inventory
 *   <li>{@link HeldItemChange} — player changes their held hotbar slot
 *   <li>{@link CreativeSlot} — player sets a slot in creative mode; cancellable
 *   <li>{@link Drag} — player drags items across inventory slots; cancellable
 * </ul>
 */
public final class PlayerInventoryEvent {

    private PlayerInventoryEvent() {}

    /** Fired when a player clicks a slot in an inventory. */
    public static class Click extends Event implements Cancellable {

        private final Player player;
        private final int slot;
        private final ItemStack itemStack;
        private boolean cancelled = false;

        public Click(Player player, int slot, ItemStack itemStack) {
            this.player = player;
            this.slot = slot;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public int slot() {
            return slot;
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

    /** Fired when a player closes an inventory. */
    public static class Close extends Event {

        private final Player player;

        public Close(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
        }
    }

    /** Fired when a player opens an inventory. */
    public static class Open extends Event {

        private final Player player;

        public Open(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
        }
    }

    /** Fired when a player changes their held hotbar slot. */
    public static class HeldItemChange extends Event {

        private final Player player;
        private final int slot;

        public HeldItemChange(Player player, int slot) {
            this.player = player;
            this.slot = slot;
        }

        public Player player() {
            return player;
        }

        public int slot() {
            return slot;
        }
    }

    /** Fired when a player sets a slot in creative mode. */
    public static class CreativeSlot extends Event implements Cancellable {

        private final Player player;
        private final int slot;
        private final ItemStack itemStack;
        private boolean cancelled = false;

        public CreativeSlot(Player player, int slot, ItemStack itemStack) {
            this.player = player;
            this.slot = slot;
            this.itemStack = itemStack;
        }

        public Player player() {
            return player;
        }

        public int slot() {
            return slot;
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

    /** Fired when a player drags items across inventory slots. */
    public static class Drag extends Event implements Cancellable {

        private final Player player;
        private boolean cancelled = false;

        public Drag(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
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
