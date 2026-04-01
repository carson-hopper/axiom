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

    private record PrioritizedHandler<T>(Consumer<T> handler, EventPriority priority) {}

    private final ConcurrentHashMap<Class<?>, List<PrioritizedHandler<?>>> handlers =
            new ConcurrentHashMap<>();

    @Override
    public <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler) {
        subscribe(eventType, handler, EventPriority.NORMAL);
    }

    @Override
    @SuppressWarnings("unchecked")
    public <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler,
                                                  EventPriority priority) {
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
        if (list == null) return;

        // Snapshot and sort by priority ordinal (LOWEST=0 fires first)
        List<PrioritizedHandler<?>> sorted = new ArrayList<>(list);
        sorted.sort(Comparator.comparingInt(ph -> ph.priority().ordinal()));

        for (PrioritizedHandler ph : sorted) {
            if (event instanceof Cancellable c && c.isCancelled()
                    && ph.priority() != EventPriority.MONITOR) {
                continue;
            }
            ph.handler().accept(event);
        }
    }
}
