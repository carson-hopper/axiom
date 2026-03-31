package com.axiommc.api.event;

/**
 * Marker interface for cancellable events.
 *
 * <p>Events that implement this interface can be cancelled. When an event
 * is cancelled, all remaining handlers (except those at {@link EventPriority#MONITOR}
 * priority) will not fire. MONITOR priority handlers always execute
 * regardless of cancellation state.
 */
public interface Cancellable {
    /**
     * Checks whether this event has been cancelled.
     *
     * @return true if cancelled, false otherwise
     */
    boolean isCancelled();

    /**
     * Sets the cancellation state of this event.
     *
     * @param cancelled true to cancel the event, false to uncancel
     */
    void setCancelled(boolean cancelled);
}
