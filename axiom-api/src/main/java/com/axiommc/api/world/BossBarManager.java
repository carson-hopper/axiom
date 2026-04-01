package com.axiommc.api.world;

import com.axiommc.api.bossbar.BossBar;

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
