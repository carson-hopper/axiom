package com.axiommc.api.event;

import java.util.function.Consumer;

/**
 * Manages event subscriptions and publishing.
 *
 * <p>The event bus allows plugins to subscribe to events and react when they
 * occur. Handlers are called with a specified priority to control execution
 * order.
 */
public interface EventBus {

    /**
     * Subscribes a handler to an event type with default priority.
     *
     * <p>The handler will be called with {@link EventPriority#NORMAL} priority.
     *
     * @param <T> the event type
     * @param eventType the event class
     * @param handler the handler to call when the event fires
     */
    <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler);

    /**
     * Subscribes a handler to an event type with a specified priority.
     *
     * @param <T> the event type
     * @param eventType the event class
     * @param handler the handler to call when the event fires
     * @param priority the priority at which this handler executes
     */
    <T extends Event> void subscribe(
        Class<T> eventType, Consumer<T> handler, EventPriority priority);

    /**
     * Unsubscribes a handler from an event type.
     *
     * @param <T> the event type
     * @param eventType the event class
     * @param handler the handler to remove
     */
    <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler);

    /**
     * Publishes an event to all subscribed handlers.
     *
     * <p>Handlers are called in priority order, with LOWEST priority handlers
     * firing first.
     *
     * @param event the event to publish
     */
    void publish(Event event);
}
