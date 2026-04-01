package com.axiommc.fabric.event;

import com.axiommc.api.event.EventListener;
import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.player.PlayerLeaveEvent;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerTickEvents;
import net.minecraft.server.MinecraftServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

/**
 * Fabric implementation of EventBus that hooks into Fabric events.
 */
public class FabricEventBus extends SimpleEventBus {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricEventBus.class);
    private boolean playerEventsSetup = false;
    private final Set<UUID> onlinePlayers = new HashSet<>();

    public FabricEventBus() {
        // Events are set up later via setupPlayerEvents() call
    }

    /**
     * Initializes player event hooks. Should be called after playerProvider is created.
     */
    public void setupPlayerEvents() {
        if (playerEventsSetup) {
            return;
        }
        playerEventsSetup = true;
        try {
            // Use server tick events to detect player join/leave
            ServerTickEvents.END_SERVER_TICK.register(this::checkPlayerChanges);
            LOGGER.info("Player event hooks registered");
        } catch (Exception e) {
            LOGGER.warn("Failed to setup player events", e);
        }
    }

    /**
     * Called every server tick to check for player join/leave events.
     */
    private void checkPlayerChanges(MinecraftServer server) {
        try {
            FabricPlayerProvider provider = getFabricPlayerProvider();
            if (provider == null) {
                return;
            }

            Set<UUID> currentPlayers = new HashSet<>();
            for (var serverPlayer : server.getPlayerList().getPlayers()) {
                UUID uuid = serverPlayer.getUUID();
                currentPlayers.add(uuid);

                // Player joined
                if (!onlinePlayers.contains(uuid)) {
                    var axiomPlayer = provider.player(uuid);
                    if (axiomPlayer.isPresent()) {
                        LOGGER.debug("Firing PlayerJoinEvent for {}", serverPlayer.getName().getString());
                        publish(new PlayerJoinEvent(axiomPlayer.get()));
                    }
                }
            }

            // Check for players who left
            for (UUID uuid : onlinePlayers) {
                if (!currentPlayers.contains(uuid)) {
                    var axiomPlayer = provider.player(uuid);
                    if (axiomPlayer.isPresent()) {
                        LOGGER.debug("Firing PlayerLeaveEvent for {}", axiomPlayer.get().name());
                        publish(new PlayerLeaveEvent(axiomPlayer.get()));
                    }
                }
            }

            onlinePlayers.clear();
            onlinePlayers.addAll(currentPlayers);
        } catch (Exception e) {
            LOGGER.debug("Error checking player changes", e);
        }
    }

    /**
     * Registers all @EventHandler methods from an event listener.
     *
     * @param listener the event listener to register
     */
    public void registerListener(EventListener listener) {
        EventHandlerScanner.registerListener(listener, this);
    }

    /**
     * Gets the FabricPlayerProvider from AxiomMod.
     */
    private FabricPlayerProvider getFabricPlayerProvider() {
        try {
            var axiom = com.axiommc.fabric.AxiomMod.getInstance();
            if (axiom != null) {
                return axiom.playerProvider();
            }
        } catch (Exception e) {
            // Silently fail if not available yet
        }
        return null;
    }
}