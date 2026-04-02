package com.axiommc.api.player;

/**
 * Represents a Minecraft game mode.
 */
public enum GameMode {
    SURVIVAL,
    CREATIVE,
    ADVENTURE,
    SPECTATOR;

    /**
     * Returns the game mode matching the given name, or null if unknown.
     *
     * @param name the game mode name (case-insensitive)
     * @return the matching game mode, or null
     */
    public static GameMode fromName(String name) {
        for (GameMode mode : values()) {
            if (mode.name().equalsIgnoreCase(name)) {
                return mode;
            }
        }
        return null;
    }
}
