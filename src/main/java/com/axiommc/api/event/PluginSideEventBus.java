package com.axiommc.api.event;

import com.axiommc.api.messaging.Channels;
import com.axiommc.api.messaging.MessageBus;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.function.Consumer;

/**
 * An {@link EventBus} that transparently forwards {@link CrossSide}-annotated events
 * to the other side (Velocity↔Fabric) via a module's {@link MessageBus}.
 *
 * <p>Each module gets its own {@code CrossSideEventBus} instance. The {@code delegate}
 * is typically the shared {@code SimpleEventBus} so all modules' subscribers hear
 * forwarded events regardless of which module forwarded them.
 *
 * <p>Loop prevention: inbound events are published via {@code delegate.publish()},
 * not {@code this.publish()}, so they are never re-forwarded.
 */
public record PluginSideEventBus(EventBus delegate, MessageBus messageBus, ClassLoader classLoader) implements EventBus {

    private static final Logger logger = LoggerFactory.getLogger(PluginSideEventBus.class);

    /**
     * @param delegate    the shared local event bus (typically {@code SimpleEventBus})
     * @param messageBus  this module's message bus used for cross-side transport
     * @param classLoader used on the receiving side to load the event class by name;
     *                    pass the module's {@code URLClassLoader} on Velocity,
     *                    or {@code moduleInstance.getClass().getClassLoader()} on Fabric
     */
    public PluginSideEventBus(EventBus delegate, MessageBus messageBus, ClassLoader classLoader) {
        this.delegate = delegate;
        this.messageBus = messageBus;
        this.classLoader = classLoader;
        messageBus.on(Channels.EVENT_FORWARD, this::handleInbound);
    }

    @Override
    public <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler) {
        delegate.subscribe(eventType, handler);
    }

    @Override
    public <T extends Event> void subscribe(Class<T> eventType, Consumer<T> handler, EventPriority priority) {
        delegate.subscribe(eventType, handler, priority);
    }

    @Override
    public <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler) {
        delegate.unsubscribe(eventType, handler);
    }

    /**
     * Publishes the event locally, then forwards it cross-side if annotated {@code @CrossSide}.
     *
     * @throws PluginSideSerializationException if the event is {@code @CrossSide} but cannot
     *                                         be serialized (unsupported field type or missing no-arg constructor)
     */
    @Override
    public void publish(Event event) {
        delegate.publish(event);
        if (event.getClass().isAnnotationPresent(CrossSide.class)) {
            forwardCrossSide(event);
        }
    }

    private void forwardCrossSide(Event event) {
        byte[] data = EventSerializer.serialize(event);
        messageBus.send(Channels.EVENT_FORWARD, data);
    }

    /**
     * Receives a forwarded event from the other side and fires it locally.
     * Calls {@code delegate.publish()} — NOT {@code this.publish()} — to prevent re-forwarding.
     */
    private void handleInbound(byte[] data) {
        try {
            Event event = EventSerializer.deserialize(data, classLoader);
            delegate.publish(event);
        } catch (ClassNotFoundException e) {
            logger.warn("Cross-side event class not found on this side: {}. " +
                    "Ensure the module JAR is deployed on both sides.", e.getMessage());
        } catch (Exception e) {
            logger.warn("Failed to deserialize cross-side event: {}", e.getMessage());
        }
    }
}
