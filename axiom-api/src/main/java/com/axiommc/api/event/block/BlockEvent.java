package com.axiommc.api.event.block;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.item.ItemStack;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.World;
import java.util.List;

public class BlockEvent {

    /**
     * Fired when a player breaks a block.
     */
    public static class Break extends Event implements Cancellable {

        private final Player player;
        private final Location blockLocation;
        private final String blockType;
        private boolean cancelled = false;

        public Break(Player player, Location blockLocation, String blockType) {
            this.player = player;
            this.blockLocation = blockLocation;
            this.blockType = blockType;
        }

        public Player player() {
            return player;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public String blockType() {
            return blockType;
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
        private final Location blockLocation;
        private final String blockType;
        private boolean cancelled = false;

        public Place(Player player, Location blockLocation, String blockType) {
            this.player = player;
            this.blockLocation = blockLocation;
            this.blockType = blockType;
        }

        public Player player() {
            return player;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public String blockType() {
            return blockType;
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

        private final World world;
        private final Location blockLocation;
        private boolean cancelled = false;

        public Explode(World world, Location blockLocation) {
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

    /**
     * Fired when a block is destroyed by fire.
     */
    public static class Burn extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private boolean cancelled = false;

        public Burn(World world, Location blockLocation) {
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

    /**
     * Fired when a block catches fire.
     */
    public static class Ignite extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final String cause;
        private boolean cancelled = false;

        public Ignite(World world, Location blockLocation, String cause) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.cause = cause;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
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

        private final World world;
        private final Location blockLocation;
        private final String blockType;
        private boolean cancelled = false;

        public Grow(World world, Location blockLocation, String blockType) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.blockType = blockType;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public String blockType() {
            return blockType;
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

        private final World world;
        private final Location blockLocation;

        public Fade(World world, Location blockLocation) {
            this.world = world;
            this.blockLocation = blockLocation;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }
    }

    /**
     * Fired when a dispenser fires.
     */
    public static class Dispense extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final ItemStack item;
        private boolean cancelled = false;

        public Dispense(World world, Location blockLocation, ItemStack item) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.item = item;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
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
        private final Location blockLocation;
        private final String[] lines;
        private boolean cancelled = false;

        public SignChange(Player player, Location blockLocation, String[] lines) {
            this.player = player;
            this.blockLocation = blockLocation;
            this.lines = lines;
        }

        public Player player() {
            return player;
        }

        public Location blockLocation() {
            return blockLocation;
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

        private final World world;
        private final Location blockLocation;
        private final int oldCurrent;
        private final int newCurrent;

        public Redstone(World world, Location blockLocation, int oldCurrent, int newCurrent) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.oldCurrent = oldCurrent;
            this.newCurrent = newCurrent;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public int oldCurrent() {
            return oldCurrent;
        }

        public int newCurrent() {
            return newCurrent;
        }
    }

    /** Fired when a piston extends and pushes blocks. */
    public static class PistonExtend extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final List<Location> affectedBlocks;
        private boolean cancelled = false;

        public PistonExtend(World world, Location blockLocation, List<Location> affectedBlocks) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.affectedBlocks = List.copyOf(affectedBlocks);
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public List<Location> affectedBlocks() {
            return affectedBlocks;
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

    /** Fired when a piston retracts and pulls blocks. */
    public static class PistonRetract extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final List<Location> affectedBlocks;
        private boolean cancelled = false;

        public PistonRetract(World world, Location blockLocation, List<Location> affectedBlocks) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.affectedBlocks = List.copyOf(affectedBlocks);
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public List<Location> affectedBlocks() {
            return affectedBlocks;
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
