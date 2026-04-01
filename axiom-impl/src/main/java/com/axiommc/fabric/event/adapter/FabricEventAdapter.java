package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.fabric.player.FabricPlayerProvider;

/**
 * Bridges a category of Fabric API callbacks to Axiom API events.
 * Each adapter registers itself with the Fabric event system and
 * publishes corresponding Axiom events on the provided event bus.
 */
public interface FabricEventAdapter {

    /**
     * Called once when all dependencies (event bus, player provider) are ready.
     * Implementations register their Fabric callbacks here.
     */
    void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider);
}
