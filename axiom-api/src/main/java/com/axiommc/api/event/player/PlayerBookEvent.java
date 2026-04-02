package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player interacts with a book.
 *
 * <p>Subtypes: {@link Edit}.
 */
public class PlayerBookEvent {

    private PlayerBookEvent() {}

    /** Fired when a player edits a book. */
    public static class Edit extends Event implements Cancellable {

        private final Player player;
        private final ItemStack book;
        private boolean cancelled = false;

        public Edit(Player player, ItemStack book) {
            this.player = player;
            this.book = book;
        }

        public Player player() {
            return player;
        }

        public ItemStack book() {
            return book;
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
