package com.axiommc.api.event.block;

import com.axiommc.api.block.Block;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
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
 *   <li>{@link NoteBlockPlay} — a note block plays a note; cancellable
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

        private final Block block;
        private final int oldPower;
        private final int newPower;

        public PowerChange(Block block, int oldPower, int newPower) {
            this.block = block;
            this.oldPower = oldPower;
            this.newPower = newPower;
        }

        public Block block() {
            return block;
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

        private final Block block;

        public ButtonPress(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
        }
    }

    /** Fired when a lever is toggled. */
    public static class LeverToggle extends Event {

        private final Block block;
        private final boolean powered;

        public LeverToggle(Block block, boolean powered) {
            this.block = block;
            this.powered = powered;
        }

        public Block block() {
            return block;
        }

        public boolean isPowered() {
            return powered;
        }
    }

    /** Fired when a pressure plate is activated. */
    public static class PressurePlateActivate extends Event {

        private final Block block;

        public PressurePlateActivate(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
        }
    }

    /** Fired when a pressure plate is deactivated. */
    public static class PressurePlateDeactivate extends Event {

        private final Block block;

        public PressurePlateDeactivate(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
        }
    }

    /** Fired when a tripwire is activated. */
    public static class TripwireActivate extends Event {

        private final Block block;

        public TripwireActivate(Block block) {
            this.block = block;
        }

        public Block block() {
            return block;
        }
    }

    /** Fired when a note block plays a note. */
    public static class NoteBlockPlay extends Event implements Cancellable {

        private final Block block;
        private final int note;
        private final int instrument;
        private boolean cancelled = false;

        public NoteBlockPlay(Block block, int note, int instrument) {
            this.block = block;
            this.note = note;
            this.instrument = instrument;
        }

        public Block block() {
            return block;
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

        private final Block block;
        private final int oldOutput;
        private final int newOutput;

        public ComparatorUpdate(Block block, int oldOutput, int newOutput) {
            this.block = block;
            this.oldOutput = oldOutput;
            this.newOutput = newOutput;
        }

        public Block block() {
            return block;
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

        private final Block block;
        private final boolean powered;
        private final int delay;

        public RepeaterUpdate(Block block, boolean powered, int delay) {
            this.block = block;
            this.powered = powered;
            this.delay = delay;
        }

        public Block block() {
            return block;
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

        private final Block block;
        private final List<Block> affectedBlocks;
        private boolean cancelled = false;

        public PistonExtend(Block block, List<Block> affectedBlocks) {
            this.block = block;
            this.affectedBlocks = List.copyOf(affectedBlocks);
        }

        public Block block() {
            return block;
        }

        public List<Block> affectedBlocks() {
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

        private final Block block;
        private final List<Block> affectedBlocks;
        private boolean cancelled = false;

        public PistonRetract(Block block, List<Block> affectedBlocks) {
            this.block = block;
            this.affectedBlocks = List.copyOf(affectedBlocks);
        }

        public Block block() {
            return block;
        }

        public List<Block> affectedBlocks() {
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
