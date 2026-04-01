package com.axiommc.fabric.mixin.net.minecraft.server;

import net.minecraft.server.rcon.RconConsoleSource;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * Integrates with JLine3 to add commands to the console history.
 * Uses reflection to access JLine3's LineReader without compile-time dependency.
 */
@Mixin(RconConsoleSource.class)
public class RconConsoleSourceMixin {

    /**
     * Hooks into the accept method to add executed commands to JLine3 history.
     */
    @Inject(method = "accept", at = @At("HEAD"), cancellable = false)
    private void addCommandToHistory(String command, CallbackInfo ci) {
        if (command == null || command.trim().isEmpty()) {
            return;
        }

        try {
            // Get lineReader field from RconConsoleSource using reflection
            Field lineReaderField = null;
            for (Field field : RconConsoleSource.class.getDeclaredFields()) {
                if (field.getType().getSimpleName().equals("LineReader")) {
                    lineReaderField = field;
                    break;
                }
            }

            if (lineReaderField == null) {
                return;
            }

            lineReaderField.setAccessible(true);
            Object lineReader = lineReaderField.get(this);

            if (lineReader == null) {
                return;
            }

            // Get the History object from LineReader
            Method getHistoryMethod = lineReader.getClass().getMethod("getHistory");
            Object history = getHistoryMethod.invoke(lineReader);

            // Call history.add(command)
            Method addMethod = history.getClass().getMethod("add", String.class);
            addMethod.invoke(history, command);
        } catch (Exception e) {
            // Silently ignore - history addition is non-critical
        }
    }
}
