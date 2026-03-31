package com.axiommc.api.event;

/**
 * Determines the execution order of event handlers.
 *
 * <p>Handlers are executed in priority order, from LOWEST to MONITOR. The
 * MONITOR priority should be used only for observation; handlers at this
 * priority should not modify the event or perform side effects.
 */
public enum EventPriority {
    /**
     * Executed first. Use for pre-processing or filtering.
     */
    LOWEST,

    /**
     * Executed before normal handlers.
     */
    LOW,

    /**
     * The default priority for most handlers.
     */
    NORMAL,

    /**
     * Executed after normal handlers.
     */
    HIGH,

    /**
     * Executed last before MONITOR. Use for cleanup or final decisions.
     */
    HIGHEST,

    /**
     * Executed after all other handlers. Use only to observe the final event
     * state; handlers should not cancel or modify the event.
     */
    MONITOR
}
