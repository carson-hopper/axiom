package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;

import java.util.UUID;

/**
 * Fired when a player profile is requested.
 */
public class GameProfileRequestEvent extends Event {

    private final String playerName;
    private UUID playerUUID;

    public GameProfileRequestEvent(String playerName) {
        this.playerName = playerName;
    }

    public String username() {
        return playerName;
    }

    public UUID uuid() {
        return playerUUID;
    }

    public void uuid(UUID playerUUID) {
        this.playerUUID = playerUUID;
    }
}
