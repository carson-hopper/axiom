package com.axiommc.api.event;

/**
 * Base class for all events in the Axiom system.
 *
 * <p>All events must extend this class to be published and subscribed to via
 * the {@link EventBus}. Implementations should provide relevant data about
 * what occurred as public fields or getter methods.
 */
public abstract class Event {
}
