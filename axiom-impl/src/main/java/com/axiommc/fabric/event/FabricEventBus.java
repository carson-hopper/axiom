package com.axiommc.fabric.event;

import com.axiommc.api.event.EventListener;
import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.fabric.event.adapter.BlockInteractAdapter;
import com.axiommc.fabric.event.adapter.CommandExecuteAdapter;
import com.axiommc.fabric.event.adapter.FabricEventAdapter;
import com.axiommc.fabric.event.adapter.PlayerChatAdapter;
import com.axiommc.fabric.event.adapter.PlayerConnectionAdapter;
import com.axiommc.fabric.event.adapter.ServerLifecycleAdapter;
import com.axiommc.fabric.player.FabricPlayerProvider;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;

/**
 * Fabric implementation of EventBus that uses adapters to bridge
 * Fabric API callbacks to Axiom API events.
 */
public class FabricEventBus extends SimpleEventBus {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricEventBus.class);
    private final List<FabricEventAdapter> adapters = new ArrayList<>();
    private boolean initialized = false;

    public FabricEventBus() {
        adapters.add(new PlayerConnectionAdapter());
        adapters.add(new ServerLifecycleAdapter());
        adapters.add(new PlayerChatAdapter());
        adapters.add(new CommandExecuteAdapter());
        adapters.add(new BlockInteractAdapter());
    }

    /**
     * Initializes all event adapters. Must be called after playerProvider is ready.
     *
     * @param playerProvider the player provider for wrapping Minecraft players
     */
    public void initialize(FabricPlayerProvider playerProvider) {
        if (initialized) {
            return;
        }
        initialized = true;

        for (FabricEventAdapter adapter : adapters) {
            try {
                adapter.register(this, playerProvider);
                LOGGER.info("Registered event adapter: {}", adapter.getClass().getSimpleName());
            } catch (Exception e) {
                LOGGER.warn("Failed to register adapter: {}", adapter.getClass().getSimpleName(), e);
            }
        }
    }

    @Override
    public void registerListener(EventListener listener) {
        EventHandlerScanner.registerListener(listener, this);
    }
}
