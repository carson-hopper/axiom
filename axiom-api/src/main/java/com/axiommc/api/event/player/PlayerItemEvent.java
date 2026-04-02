package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player drops or picks up an item.
 *
 * <p>Subtypes: {@link Drop}, {@link Pickup}.
 */
public class PlayerItemEvent {

    private PlayerItemEvent() {}

    /** Fired when a player drops an item. */
    public static class Drop extends Event implements Cancellable {

        private final Player player;
        private final String item;
        private boolean cancelled = false;

        public Drop(Player player, String item) {
            this.player = player;
            this.item = item;
        }

        public Player player() {
            return player;
        }

        public String item() {
            return item;
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

    /** Fired when a player picks up an item. */
    public static class Pickup extends Event implements Cancellable {

        private final Player player;
        private final String item;
        private final int amount;
        private boolean cancelled = false;

        public Pickup(Player player, String item, int amount) {
            this.player = player;
            this.item = item;
            this.amount = amount;
        }

        public Player player() {
            return player;
        }

        public String item() {
            return item;
        }

        public int amount() {
            return amount;
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
