package com.axiommc.api.event.block;

import com.axiommc.api.block.Block;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Player;

public class BlockEvent {

    /**
     * Fired when a player breaks a block.
     */
    public static class Break extends Event implements Cancellable {

        private final Player player;
        private final Block block;
        private boolean cancelled = false;

        public Break(Player player, Block block) {
            this.player = player;
            this.block = block;
        }

        public Player player() {
            return player;
        }

        public Block block() {
            return block;
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

    /**
     * Fired when a player places a block.
     */
    public static class Place extends Event implements Cancellable {

        private final Player player;
        private final Block block;
        private boolean cancelled = false;

        public Place(Player player, Block block) {
            this.player = player;
            this.block = block;
        }

        public Player player() {
            return player;
        }

        public Block block() {
            return block;
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

    /**
     * Fired when a block explodes.
     */
    public static class Explode extends Event implements Cancellable {

        private final Block block;
        private boolean cancelled = false;

        public Explode(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
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

    /**
     * Fired when a block is destroyed by fire.
     */
    public static class Burn extends Event implements Cancellable {

        private final Block block;
        private boolean cancelled = false;

        public Burn(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
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

    /**
     * Fired when a block catches fire.
     */
    public static class Ignite extends Event implements Cancellable {

        private final Block block;
        private final String cause;
        private boolean cancelled = false;

        public Ignite(Block block, String cause) {
            this.block = block;
            this.cause = cause;
        }

        public Block block() {
            return block;
        }

        public String cause() {
            return cause;
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

    /**
     * Fired when a crop or plant grows.
     */
    public static class Grow extends Event implements Cancellable {

        private final Block block;
        private boolean cancelled = false;

        public Grow(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
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

    /**
     * Fired when a block fades (e.g. ice melting).
     */
    public static class Fade extends Event {

        private final Block block;

        public Fade(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
        }
    }

    /**
     * Fired when a dispenser fires.
     */
    public static class Dispense extends Event implements Cancellable {

        private final Block block;
        private final ItemStack item;
        private boolean cancelled = false;

        public Dispense(Block block, ItemStack item) {
            this.block = block;
            this.item = item;
        }

        public Block block() {
            return block;
        }

        public ItemStack item() {
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

    /**
     * Fired when a player edits a sign.
     */
    public static class SignChange extends Event implements Cancellable {

        private final Player player;
        private final Block block;
        private final String[] lines;
        private boolean cancelled = false;

        public SignChange(Player player, Block block, String[] lines) {
            this.player = player;
            this.block = block;
            this.lines = lines;
        }

        public Player player() {
            return player;
        }

        public Block block() {
            return block;
        }

        public String[] lines() {
            return lines;
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

    /**
     * Fired when a redstone signal changes.
     */
    public static class Redstone extends Event {

        private final Block block;
        private final int oldCurrent;
        private final int newCurrent;

        public Redstone(Block block, int oldCurrent, int newCurrent) {
            this.block = block;
            this.oldCurrent = oldCurrent;
            this.newCurrent = newCurrent;
        }

        public Block block() {
            return block;
        }

        public int oldCurrent() {
            return oldCurrent;
        }

        public int newCurrent() {
            return newCurrent;
        }
    }
}
