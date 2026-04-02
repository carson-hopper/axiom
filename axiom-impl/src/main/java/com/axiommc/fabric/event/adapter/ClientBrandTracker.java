package com.axiommc.fabric.event.adapter;

import com.axiommc.fabric.mixin.net.minecraft.server.network.ServerCommonPacketListenerAccessor;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import net.minecraft.network.Connection;
import net.minecraft.server.network.ServerCommonPacketListenerImpl;

/**
 * Tracks client brand strings received during the configuration phase
 * (before a ServerPlayer exists). The brand is stored when the
 * {@code BrandPayload} arrives and consumed when the player joins.
 */
public final class ClientBrandTracker {

    private static final Map<Integer, String> PENDING_BRANDS = new ConcurrentHashMap<>();

    private ClientBrandTracker() {}

    /**
     * Stores a brand for the given listener's connection.
     */
    public static void store(ServerCommonPacketListenerImpl listener, String brand) {
        Connection connection = ((ServerCommonPacketListenerAccessor) listener).connection();
        PENDING_BRANDS.put(System.identityHashCode(connection), brand);
    }

    /**
     * Retrieves and removes the pending brand for the given listener.
     *
     * @return the brand string, or null if none was stored
     */
    public static String consume(ServerCommonPacketListenerImpl listener) {
        Connection connection = ((ServerCommonPacketListenerAccessor) listener).connection();
        return PENDING_BRANDS.remove(System.identityHashCode(connection));
    }
}
