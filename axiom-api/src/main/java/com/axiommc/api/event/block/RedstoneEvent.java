package com.axiommc.api.event.block;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import java.util.List;

/**
 * Events fired for redstone-related block interactions.
 *
 * <ul>
 *   <li>{@link PowerChange} — a block's redstone power level changes
 *   <li>{@link ButtonPress} — a button is pressed
 *   <li>{@link LeverToggle} — a lever is toggled
 *   <li>{@link PressurePlateActivate} — a pressure plate is activated
 *   <li>{@link PressurePlateDeactivate} — a pressure plate is deactivated
 *   <li>{@link TripwireActivate} — a tripwire is activated
 *   <li>{@link NoteBlockPlay} — a note block plays a note
 *   <li>{@link ComparatorUpdate} — a comparator output changes
 *   <li>{@link RepeaterUpdate} — a repeater state changes
 *   <li>{@link PistonExtend} — a piston extends; cancellable
 *   <li>{@link PistonRetract} — a piston retracts; cancellable
 * </ul>
 */
public final class RedstoneEvent {

    private RedstoneEvent() {}

    /** Fired when a block's redstone power level changes. */
    public static class PowerChange extends Event {

        private final World world;
        private final Location blockLocation;
        private final int oldPower;
        private final int newPower;

        public PowerChange(World world, Location blockLocation, int oldPower, int newPower) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.oldPower = oldPower;
            this.newPower = newPower;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public int oldPower() {
            return oldPower;
        }

        public int newPower() {
            return newPower;
        }
    }

    /** Fired when a button is pressed. */
    public static class ButtonPress extends Event {

        private final World world;
        private final Location blockLocation;
        private final String blockType;

        public ButtonPress(World world, Location blockLocation, String blockType) {
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
    }

    /** Fired when a lever is toggled. */
    public static class LeverToggle extends Event {

        private final World world;
        private final Location blockLocation;
        private final boolean powered;

        public LeverToggle(World world, Location blockLocation, boolean powered) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.powered = powered;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public boolean isPowered() {
            return powered;
        }
    }

    /** Fired when a pressure plate is activated. */
    public static class PressurePlateActivate extends Event {

        private final World world;
        private final Location blockLocation;

        public PressurePlateActivate(World world, Location blockLocation) {
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

    /** Fired when a pressure plate is deactivated. */
    public static class PressurePlateDeactivate extends Event {

        private final World world;
        private final Location blockLocation;

        public PressurePlateDeactivate(World world, Location blockLocation) {
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

    /** Fired when a tripwire is activated. */
    public static class TripwireActivate extends Event {

        private final World world;
        private final Location blockLocation;

        public TripwireActivate(World world, Location blockLocation) {
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

    /** Fired when a note block plays a note. */
    public static class NoteBlockPlay extends Event implements Cancellable {

        private final World world;
        private final Location blockLocation;
        private final int note;
        private final int instrument;
        private boolean cancelled = false;

        public NoteBlockPlay(World world, Location blockLocation, int note, int instrument) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.note = note;
            this.instrument = instrument;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public int note() {
            return note;
        }

        public int instrument() {
            return instrument;
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

    /** Fired when a comparator output changes. */
    public static class ComparatorUpdate extends Event {

        private final World world;
        private final Location blockLocation;
        private final int oldOutput;
        private final int newOutput;

        public ComparatorUpdate(World world, Location blockLocation, int oldOutput, int newOutput) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.oldOutput = oldOutput;
            this.newOutput = newOutput;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public int oldOutput() {
            return oldOutput;
        }

        public int newOutput() {
            return newOutput;
        }
    }

    /** Fired when a repeater state changes. */
    public static class RepeaterUpdate extends Event {

        private final World world;
        private final Location blockLocation;
        private final boolean powered;
        private final int delay;

        public RepeaterUpdate(World world, Location blockLocation, boolean powered, int delay) {
            this.world = world;
            this.blockLocation = blockLocation;
            this.powered = powered;
            this.delay = delay;
        }

        public World world() {
            return world;
        }

        public Location blockLocation() {
            return blockLocation;
        }

        public boolean isPowered() {
            return powered;
        }

        public int delay() {
            return delay;
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
