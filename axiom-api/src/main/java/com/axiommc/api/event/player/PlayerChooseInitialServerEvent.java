package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.world.Server;

import java.util.UUID;

/**
 * Fired when a player is choosing their initial server.
 * Can be used to determine which server a new player joins first.
 */
public class PlayerChooseInitialServerEvent extends Event {

    private final String playerName;
    private final UUID playerUUID;
    private Server initialServer;

    public PlayerChooseInitialServerEvent(String playerName, UUID playerUUID) {
        this.playerName = playerName;
        this.playerUUID = playerUUID;
    }

    public String username() {
        return playerName;
    }

    public UUID uuid() {
        return playerUUID;
    }

    public Server initialServer() {
        return initialServer;
    }

    public void initialServer(Server server) {
        this.initialServer = server;
    }
}
