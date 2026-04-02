package com.axiommc.api.event;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.function.Consumer;

/**
 * Thread-safe priority-aware event bus implementation.
 * Platform event buses (FabricEventBus, VelocityEventBus) extend this class.
 */
public class SimpleEventBus implements EventBus {

    private record PrioritizedHandler<T extends Event>(
        Consumer<T> handler, EventPriority priority) {}

    private final ConcurrentHashMap<Class<?>, List<PrioritizedHandler<?>>> handlers =
        new ConcurrentHashMap<>();

    @Override
    public <T extends Event> EventSubscription<T> listen(Class<T> eventType) {
        return new EventSubscription<>(this, eventType);
    }

    /**
     * Registers a handler directly. Used internally by {@link EventSubscription}.
     */
    public <T extends Event> void addHandler(
        Class<T> eventType, Consumer<T> handler, EventPriority priority) {
        handlers.computeIfAbsent(eventType, k -> new CopyOnWriteArrayList<>())
            .add(new PrioritizedHandler<>(handler, priority));
    }

    @Override
    public <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler) {
        List<PrioritizedHandler<?>> list = handlers.get(eventType);
        if (list != null) {
            list.removeIf(ph -> ph.handler() == handler);
        }
    }

    @Override
    @SuppressWarnings({"unchecked", "rawtypes"})
    public void publish(Event event) {
        List<PrioritizedHandler<?>> list = handlers.get(event.getClass());
        if (list == null) {
            return;
        }

        List<PrioritizedHandler<?>> sorted = new ArrayList<>(list);
        sorted.sort(Comparator.comparingInt(ph -> ph.priority().ordinal()));

        for (PrioritizedHandler ph : sorted) {
            if (event instanceof Cancellable c
                && c.isCancelled()
                && ph.priority() != EventPriority.MONITOR) {
                continue;
            }
            ph.handler().accept(event);
        }
    }

    /**
     * Registers all @EventHandler methods from an event listener.
     * Default implementation does nothing; subclasses should override.
     */
    public void registerListener(EventListener listener) {
        // Default implementation - subclasses should override
    }
}
