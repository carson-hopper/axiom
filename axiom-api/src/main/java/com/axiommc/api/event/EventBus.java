package com.axiommc.api.event;

import java.util.function.Consumer;

/**
 * Manages event subscriptions and publishing.
 *
 * <p>Use {@link #listen(Class)} to subscribe to events with optional
 * filtering and priority:
 *
 * <pre>{@code
 * eventBus.listen(BlockEvent.Break.class)
 *     .filter(event -> event.block().type() == Material.STONE)
 *     .handler(event -> { ... });
 * }</pre>
 */
public interface EventBus {

    /**
     * Creates a subscription builder for the given event type.
     *
     * @param <T> the event type
     * @param eventType the event class
     * @return a subscription builder
     */
    <T extends Event> EventSubscription<T> listen(Class<T> eventType);

    /**
     * Unsubscribes a handler from an event type.
     *
     * @param <T> the event type
     * @param eventType the event class
     * @param handler the handler to remove
     */
    <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler);

    /**
     * Registers all {@code @EventHandler} methods from a class-based listener.
     *
     * @param listener the event listener instance
     */
    void registerListener(EventListener listener);

    /**
     * Publishes an event to all subscribed handlers.
     *
     * @param event the event to publish
     */
    void publish(Event event);
}
