package com.axiommc.api.event;

/**
 * Implemented by Event subclasses that can be cancelled.
 * When an event is cancelled, all subsequent handlers except those
 * at MONITOR priority will not fire. MONITOR handlers always fire
 * regardless of cancellation state.
 */
public interface Cancellable {
    boolean isCancelled();
    void setCancelled(boolean cancelled);
}
