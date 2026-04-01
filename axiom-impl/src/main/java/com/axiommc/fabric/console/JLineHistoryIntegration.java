package com.axiommc.fabric.console;

import net.minecraft.server.MinecraftServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * Integrates with JLine3's in-memory history to make commands available in the current session.
 * Uses reflection to avoid compile-time JLine3 dependency.
 */
public final class JLineHistoryIntegration {

    private static final Logger LOGGER = LoggerFactory.getLogger(JLineHistoryIntegration.class);
    private static Object history;
    private static Method historyAddMethod;

    private JLineHistoryIntegration() {}

    /**
     * Initializes JLine3 integration by finding and caching the History and its add method.
     */
    public static void initialize(MinecraftServer server) {
        try {
            // Try to find the RconConsoleSource from the server
            Field consoleSourceField = null;
            for (Field field : MinecraftServer.class.getDeclaredFields()) {
                if (field.getType().getSimpleName().equals("RconConsoleSource")) {
                    consoleSourceField = field;
                    break;
                }
            }

            if (consoleSourceField == null) {
                LOGGER.debug("RconConsoleSource not found in MinecraftServer");
                return;
            }

            consoleSourceField.setAccessible(true);
            Object consoleSource = consoleSourceField.get(server);

            if (consoleSource == null) {
                LOGGER.debug("RconConsoleSource instance is null");
                return;
            }

            // Find the LineReader field in RconConsoleSource
            Field lineReaderField = null;
            for (Field field : consoleSource.getClass().getDeclaredFields()) {
                if (field.getType().getSimpleName().equals("LineReader")) {
                    lineReaderField = field;
                    break;
                }
            }

            if (lineReaderField == null) {
                LOGGER.debug("LineReader not found in RconConsoleSource");
                return;
            }

            lineReaderField.setAccessible(true);
            Object lineReader = lineReaderField.get(consoleSource);

            if (lineReader == null) {
                LOGGER.debug("LineReader instance is null");
                return;
            }

            // Get History object and cache it along with the add method
            Method getHistoryMethod = lineReader.getClass().getMethod("getHistory");
            history = getHistoryMethod.invoke(lineReader);
            historyAddMethod = history.getClass().getMethod("add", String.class);

            LOGGER.debug("JLine3 history integration initialized successfully");
        } catch (Exception e) {
            LOGGER.debug("Failed to initialize JLine3 integration: {}", e.getMessage());
        }
    }

    /**
     * Adds a command to JLine3's in-memory history.
     * Makes it available via up arrow in the current session.
     */
    public static void addCommand(String command) {
        if (historyAddMethod == null || history == null) {
            return;
        }

        if (command == null || command.trim().isEmpty()) {
            return;
        }

        try {
            historyAddMethod.invoke(history, command);
        } catch (Exception e) {
            LOGGER.debug("Failed to add command to JLine3 history", e);
        }
    }
}
