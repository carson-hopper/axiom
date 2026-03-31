package com.axiommc.api.bossbar;

/**
 * Style options for boss bars.
 *
 * <p>Styles determine how the boss bar is rendered: as a continuous bar
 * or as segments with notches.
 */
public enum BossBarStyle {
    /**
     * Solid progress bar style.
     */
    PROGRESS,

    /**
     * Segmented style with 6 notches.
     */
    NOTCHED_6,

    /**
     * Segmented style with 10 notches.
     */
    NOTCHED_10,

    /**
     * Segmented style with 12 notches.
     */
    NOTCHED_12,

    /**
     * Segmented style with 20 notches.
     */
    NOTCHED_20
}
