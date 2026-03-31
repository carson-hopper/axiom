package com.axiommc.api.event;

import java.util.function.Consumer;

public interface EventBus {

    <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler);

    <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler, EventPriority priority);

    <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler);

    void publish(Event event);
}
