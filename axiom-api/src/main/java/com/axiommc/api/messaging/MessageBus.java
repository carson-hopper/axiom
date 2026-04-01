package com.axiommc.api.messaging;

import java.util.function.Consumer;

public interface MessageBus {

    void send(String channel, byte[] data);

    void on(String channel, Consumer<byte[]> handler);

    void off(String channel);

    /**
     * Called by the channel handler when a message arrives from the proxy for
     * this module. The default no-op is overridden by platform implementations.
     */
    default void handleInbound(String channel, byte[] data) {}
}
