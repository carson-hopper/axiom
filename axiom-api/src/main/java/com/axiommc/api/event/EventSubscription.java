package com.axiommc.api.event;

import java.util.function.Consumer;
import java.util.function.Predicate;

/**
 * Builder for creating filtered event subscriptions.
 *
 * <p>Usage:
 * <pre>{@code
 * eventBus.listen(BlockEvent.Break.class)
 *     .filter(event -> event.block().type() == Material.STONE)
 *     .priority(EventPriority.HIGH)
 *     .handler(event -> player.sendMessage("Stone broken!"));
 * }</pre>
 *
 * @param <T> the event type
 */
public final class EventSubscription<T extends Event> {

    private final EventBus eventBus;
    private final Class<T> eventType;
    private Predicate<T> filter;
    private EventPriority priority = EventPriority.NORMAL;

    EventSubscription(EventBus eventBus, Class<T> eventType) {
        this.eventBus = eventBus;
        this.eventType = eventType;
    }

    /**
     * Adds a filter predicate. Only events matching the predicate
     * will be passed to the handler.
     *
     * @param filter the filter predicate
     * @return this builder
     */
    public EventSubscription<T> filter(Predicate<T> filter) {
        this.filter = filter;
        return this;
    }

    /**
     * Sets the priority for this subscription.
     *
     * @param priority the event priority
     * @return this builder
     */
    public EventSubscription<T> priority(EventPriority priority) {
        this.priority = priority;
        return this;
    }

    /**
     * Registers the handler, completing the subscription.
     *
     * @param handler the event handler
     */
    public void handler(Consumer<T> handler) {
        Consumer<T> finalHandler;
        if (filter != null) {
            Predicate<T> capturedFilter = filter;
            finalHandler = event -> {
                if (capturedFilter.test(event)) {
                    handler.accept(event);
                }
            };
        } else {
            finalHandler = handler;
        }
        if (eventBus instanceof SimpleEventBus simpleEventBus) {
            simpleEventBus.addHandler(eventType, finalHandler, priority);
        }
    }
}
