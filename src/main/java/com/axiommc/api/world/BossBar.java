package com.axiommc.api.world;

import com.axiommc.api.player.Player;

/**
 * A boss bar displayed to players.
 */
public interface BossBar {
    /**
     * Create a boss bar specification.
     */
    interface Spec {
        static Spec of(Object displayName) {
            // Minimal implementation
            return new Spec() {};
        }
    }

    /**
     * Get the boss bar's unique identifier.
     */
    String id();

    /**
     * Check if this boss bar is active.
     */
    boolean isActive();

    /**
     * Set the progress (0.0 - 1.0).
     */
    void process(float progress);

    /**
     * Add a player to this boss bar.
     */
    void addPlayer(Player player);

    /**
     * Remove a player from this boss bar.
     */
    void removePlayer(Player player);

    /**
     * Destroy this boss bar.
     */
    void destroy();
}
