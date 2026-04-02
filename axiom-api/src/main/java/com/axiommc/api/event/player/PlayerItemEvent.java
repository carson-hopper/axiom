package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player drops or picks up an item.
 *
 * <ul>
 *   <li>{@link Drop} — player drops an item stack; cancellable
 *   <li>{@link Pickup} — player picks up an item stack; cancellable
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
}
