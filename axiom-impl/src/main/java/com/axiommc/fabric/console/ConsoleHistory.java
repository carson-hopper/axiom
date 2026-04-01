package com.axiommc.fabric.console;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.List;
import com.axiommc.fabric.Axiom;

/**
 * Manages console command history by storing commands to the JLine3 history file.
 * JLine3 (used by the Minecraft server console) reads from .jline_history automatically.
 */
public final class ConsoleHistory {

    private static final String HISTORY_FILENAME = ".jline_history";
    private static File historyFile;

    private ConsoleHistory() {}

    /**
     * Initializes the console history file in the user's home directory.
     * JLine3 looks for .jline_history in the home directory for console history.
     *
     * @param workDir ignored, kept for API compatibility
     */
    public static void initialize(File workDir) {
        File homeDir = new File(System.getProperty("user.home"));
        historyFile = new File(homeDir, HISTORY_FILENAME);

        try {
            if (!historyFile.exists()) {
                historyFile.createNewFile();
                Axiom.logger().debug("Created JLine3 history file at {}", historyFile.getAbsolutePath());
            }
        } catch (IOException e) {
            Axiom.logger().warn("Failed to initialize console history in home directory", e);
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
            Axiom.logger().debug("Failed to append to console history", e);
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
            Axiom.logger().debug("Failed to read console history", e);
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
                Axiom.logger().debug("Failed to clear console history", e);
            }
        }
    }
}
