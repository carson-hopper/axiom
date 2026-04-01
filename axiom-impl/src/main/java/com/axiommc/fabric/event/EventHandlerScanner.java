package com.axiommc.fabric.event;

import com.axiommc.api.event.Event;
import com.axiommc.api.event.EventListener;
import com.axiommc.api.event.EventPriority;
import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.annotation.EventHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.reflect.Method;
import java.lang.reflect.Parameter;

/**
 * Scans EventListener classes and registers their @EventHandler methods.
 */
public final class EventHandlerScanner {

    private static final Logger LOGGER = LoggerFactory.getLogger(EventHandlerScanner.class);

    private EventHandlerScanner() {}

    /**
     * Registers all @EventHandler methods from an EventListener instance.
     *
     * @param listener the event listener instance
     * @param eventBus the event bus to register handlers with
     */
    public static void registerListener(EventListener listener, SimpleEventBus eventBus) {
        if (listener == null || eventBus == null) {
            return;
        }

        Class<?> listenerClass = listener.getClass();
        int handlersRegistered = 0;

        for (Method method : listenerClass.getDeclaredMethods()) {
            if (!method.isAnnotationPresent(EventHandler.class)) {
                continue;
            }

            // Validate method signature
            Parameter[] parameters = method.getParameters();
            if (parameters.length != 1) {
                LOGGER.warn("Event handler method {} has {} parameters, expected 1",
                    method.getName(), parameters.length);
                continue;
            }

            Class<?> eventType = parameters[0].getType();
            if (!Event.class.isAssignableFrom(eventType)) {
                LOGGER.warn("Event handler method {} parameter is not an Event: {}",
                    method.getName(), eventType.getName());
                continue;
            }

            EventHandler annotation = method.getAnnotation(EventHandler.class);
            EventPriority priority = annotation.priority();

            // Register the handler
            try {
                method.setAccessible(true);
                registerHandler(listener, method, eventType, priority, eventBus);
                handlersRegistered++;
                LOGGER.debug("Registered event handler {}.{} for {} with priority {}",
                    listenerClass.getSimpleName(), method.getName(),
                    eventType.getSimpleName(), priority);
            } catch (Exception e) {
                LOGGER.warn("Failed to register event handler {}.{}",
                    listenerClass.getSimpleName(), method.getName(), e);
            }
        }

        if (handlersRegistered > 0) {
            LOGGER.debug("Registered {} event handlers from {}",
                handlersRegistered, listenerClass.getSimpleName());
        }
    }

    /**
     * Registers a single event handler method with the event bus.
     */
    @SuppressWarnings("unchecked")
    private static void registerHandler(EventListener listener, Method method,
                                        Class<?> eventType, EventPriority priority,
                                        SimpleEventBus eventBus) throws Exception {
        eventBus.subscribe(
            (Class<? extends Event>) eventType,
            event -> {
                try {
                    method.invoke(listener, event);
                } catch (Exception e) {
                    LOGGER.warn("Error invoking event handler {}.{}",
                        listener.getClass().getSimpleName(), method.getName(), e);
                }
            },
            priority
        );
    }
}
