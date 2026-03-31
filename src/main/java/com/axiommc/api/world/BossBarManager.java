package com.axiommc.api.world;

/**
 * Manager for boss bars.
 */
public interface BossBarManager {
    /**
     * Create a new boss bar.
     */
    BossBar create(BossBar.Spec spec);

    /**
     * Get an existing boss bar by ID.
     */
    BossBar get(String id);
}
