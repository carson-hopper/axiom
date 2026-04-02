package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.block.RedstoneEvent;
import com.axiommc.fabric.block.FabricBlock;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import java.util.List;
import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.Level;

/**
 * Provides static hooks for redstone events fired from mixins.
 * Currently supports NoteBlockPlay; other redstone event subtypes
 * are defined in the API but not yet wired to mixins.
 */
public class RedstoneEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus bus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        bus = eventBus;
    }

    /**
     * Called from NoteBlockMixin when a note block plays a note.
     * Returns true if the note should play (not cancelled).
     *
     * @param level      the world level
     * @param pos        the block position
     * @param note       the note value (0-24)
     * @param instrument the instrument ordinal
     * @return true if the note should play
     */
    public static boolean onNoteBlockPlay(Level level, BlockPos pos, int note, int instrument) {
        if (bus == null || !(level instanceof ServerLevel serverLevel)) {
            return true;
        }
        FabricWorld world = new FabricWorld(serverLevel);
        FabricBlock block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), world);
        RedstoneEvent.NoteBlockPlay event =
            new RedstoneEvent.NoteBlockPlay(block, note, instrument);
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from PistonBaseBlockMixin when a piston extends.
     * Returns true if the extension should proceed (not cancelled).
     *
     * @param serverLevel the server world
     * @param pos         the piston block position
     * @return true if the piston should extend
     */
    public static boolean onPistonExtend(ServerLevel serverLevel, BlockPos pos) {
        if (bus == null) {
            return true;
        }
        FabricWorld world = new FabricWorld(serverLevel);
        FabricBlock block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), world);
        RedstoneEvent.PistonExtend event = new RedstoneEvent.PistonExtend(block, List.of());
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from PistonBaseBlockMixin when a piston retracts.
     * Returns true if the retraction should proceed (not cancelled).
     *
     * @param serverLevel the server world
     * @param pos         the piston block position
     * @return true if the piston should retract
     */
    public static boolean onPistonRetract(ServerLevel serverLevel, BlockPos pos) {
        if (bus == null) {
            return true;
        }
        FabricWorld world = new FabricWorld(serverLevel);
        FabricBlock block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), world);
        RedstoneEvent.PistonRetract event = new RedstoneEvent.PistonRetract(block, List.of());
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from ButtonBlockMixin when a button is pressed.
     *
     * @param serverLevel the server world
     * @param pos         the button block position
     */
    public static void onButtonPress(ServerLevel serverLevel, BlockPos pos) {
        if (bus == null) {
            return;
        }
        FabricWorld world = new FabricWorld(serverLevel);
        FabricBlock block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), world);
        RedstoneEvent.ButtonPress event = new RedstoneEvent.ButtonPress(block);
        bus.publish(event);
    }

    /**
     * Called from LeverBlockMixin when a lever is toggled.
     *
     * @param serverLevel the server world
     * @param pos         the lever block position
     * @param powered     the new powered state after toggling
     */
    public static void onLeverToggle(ServerLevel serverLevel, BlockPos pos, boolean powered) {
        if (bus == null) {
            return;
        }
        FabricWorld world = new FabricWorld(serverLevel);
        FabricBlock block = new FabricBlock(serverLevel, pos.getX(), pos.getY(), pos.getZ(), world);
        RedstoneEvent.LeverToggle event = new RedstoneEvent.LeverToggle(block, powered);
        bus.publish(event);
    }
}
