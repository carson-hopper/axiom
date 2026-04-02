package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player uses a bucket.
 *
 * <p>Subtypes: {@link Empty}, {@link Fill}.
 */
public class PlayerBucketEvent {

    private PlayerBucketEvent() {}

    /** Fired when a player empties a bucket. */
    public static class Empty extends Event implements Cancellable {

        private final Player player;
        private final ItemStack bucket;
        private final Location blockLocation;
        private boolean cancelled = false;

        public Empty(Player player, ItemStack bucket, Location blockLocation) {
            this.player = player;
            this.bucket = bucket;
            this.blockLocation = blockLocation;
        }

        public Player player() {
            return player;
        }

        public ItemStack bucket() {
            return bucket;
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

    /** Fired when a player fills a bucket. */
    public static class Fill extends Event implements Cancellable {

        private final Player player;
        private final ItemStack bucket;
        private final Location blockLocation;
        private boolean cancelled = false;

        public Fill(Player player, ItemStack bucket, Location blockLocation) {
            this.player = player;
            this.bucket = bucket;
            this.blockLocation = blockLocation;
        }

        public Player player() {
            return player;
        }

        public ItemStack bucket() {
            return bucket;
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
}
