package com.axiommc.fabric.mixin.net.minecraft.world.level.block;

import com.axiommc.fabric.event.adapter.RedstoneEventAdapter;
import net.minecraft.core.BlockPos;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.NoteBlock;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.block.state.properties.NoteBlockInstrument;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts note block playing to fire RedstoneEvent.NoteBlockPlay.
 */
@Mixin(value = NoteBlock.class, remap = false)
public abstract class NoteBlockMixin {

    @Inject(method = "playNote", at = @At("HEAD"), cancellable = true)
    private void onPlayNote(
        Entity entity, BlockState state, Level level, BlockPos pos, CallbackInfo callbackInfo) {
        if (level.isClientSide()) {
            return;
        }
        int note = state.getValue(NoteBlock.NOTE);
        NoteBlockInstrument instrument = state.getValue(NoteBlock.INSTRUMENT);

        if (!RedstoneEventAdapter.onNoteBlockPlay(level, pos, note, instrument.ordinal())) {
            callbackInfo.cancel();
        }
    }
}
