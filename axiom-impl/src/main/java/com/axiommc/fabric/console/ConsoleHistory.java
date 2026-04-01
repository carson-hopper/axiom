package com.axiommc.fabric.console;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.List;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Manages console command history by storing commands to a file that JLine3 uses.
 * JLine3 (used by the Minecraft server console) automatically reads from .jline_history.
 */
public final class ConsoleHistory {

    private static final Logger LOGGER = LoggerFactory.getLogger(ConsoleHistory.class);
    private static final String HISTORY_FILENAME = ".axiom_console_history";
    private static File historyFile;

    private ConsoleHistory() {}

    /**
     * Initializes the console history file in the given directory.
     *
     * @param workDir the server's working directory
     */
    public static void initialize(File workDir) {
        historyFile = new File(workDir, HISTORY_FILENAME);
        try {
            if (!historyFile.exists()) {
                historyFile.createNewFile();
                LOGGER.debug("Created console history file at {}", historyFile.getAbsolutePath());
            }
        } catch (IOException e) {
            LOGGER.warn("Failed to create console history file", e);
        }
    }

    /**
     * Adds a command to the console history file.
     * JLine3 will automatically detect this and make it available via arrow key navigation.
     *
     * @param command the command to add (should not include leading /)
     */
    public static void addCommand(String command) {
        if (historyFile == null) {
            return;
        }

        if (command == null || command.trim().isEmpty()) {
            return;
        }

        try {
            Path path = historyFile.toPath();
            String trimmedCommand = command.trim();

            // Append command with newline
            Files.write(path, (trimmedCommand + "\n").getBytes(StandardCharsets.UTF_8),
                    StandardOpenOption.APPEND, StandardOpenOption.CREATE);
        } catch (IOException e) {
            LOGGER.debug("Failed to append to console history", e);
        }
    }

    /**
     * Gets all commands from the history file.
     * Useful for displaying history or migrating to other formats.
     *
     * @return list of commands, or empty list if file doesn't exist
     */
    public static List<String> getHistory() {
        if (historyFile == null || !historyFile.exists()) {
            return new ArrayList<>();
        }

        try {
            return Files.readAllLines(historyFile.toPath(), StandardCharsets.UTF_8);
        } catch (IOException e) {
            LOGGER.debug("Failed to read console history", e);
            return new ArrayList<>();
        }
    }

    /**
     * Clears all history (for a fresh start).
     */
    public static void clear() {
        if (historyFile != null && historyFile.exists()) {
            try {
                Files.write(historyFile.toPath(), new byte[0]);
            } catch (IOException e) {
                LOGGER.debug("Failed to clear console history", e);
            }
        }
    }
}
