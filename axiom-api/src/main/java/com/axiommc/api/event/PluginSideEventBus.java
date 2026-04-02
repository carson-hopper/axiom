package com.axiommc.api.event;

import com.axiommc.api.messaging.Channels;
import com.axiommc.api.messaging.MessageBus;
import java.util.function.Consumer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * An {@link EventBus} that transparently forwards {@link CrossSide}-annotated events
 * to the other side (Velocity/Fabric) via a module's {@link MessageBus}.
 */
public record PluginSideEventBus(EventBus delegate, MessageBus messageBus, ClassLoader classLoader)
    implements EventBus {

    private static final Logger logger = LoggerFactory.getLogger(PluginSideEventBus.class);

    public PluginSideEventBus(EventBus delegate, MessageBus messageBus, ClassLoader classLoader) {
        this.delegate = delegate;
        this.messageBus = messageBus;
        this.classLoader = classLoader;
        messageBus.on(Channels.EVENT_FORWARD, this::handleInbound);
    }

    @Override
    public <T extends Event> EventSubscription<T> listen(Class<T> eventType) {
        return delegate.listen(eventType);
    }

    @Override
    public <T extends Event> void unsubscribe(Class<T> eventType, Consumer<T> handler) {
        delegate.unsubscribe(eventType, handler);
    }

    @Override
    public void registerListener(EventListener listener) {
        delegate.registerListener(listener);
    }

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

    private void handleInbound(byte[] data) {
        try {
            Event event = EventSerializer.deserialize(data, classLoader);
            delegate.publish(event);
        } catch (ClassNotFoundException exception) {
            logger.warn("Cross-side event class not found: {}", exception.getMessage());
        } catch (Exception exception) {
            logger.warn("Failed to deserialize cross-side event: {}", exception.getMessage());
        }
    }
}
