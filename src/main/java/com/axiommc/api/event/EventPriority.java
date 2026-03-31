package com.axiommc.api.event;

/**
 * Determines the order in which event handlers fire.
 * LOWEST fires first; MONITOR fires last and should only observe (not cancel) events.
 */
public enum EventPriority {
    LOWEST,
    LOW,
    NORMAL,
    HIGH,
    HIGHEST,
    MONITOR
}
