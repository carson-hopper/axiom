package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a resource pack is requested for a player.
 */
public class ServerResourcePackRequestEvent extends Event {

    private final Player player;
    private final String packUrl;
    private final String packHash;
    private final boolean required;

    public ServerResourcePackRequestEvent(Player player, String packUrl, String packHash, boolean required) {
        this.player = player;
        this.packUrl = packUrl;
        this.packHash = packHash;
        this.required = required;
    }

    public Player player() {
        return player;
    }

    public String packUrl() {
        return packUrl;
    }

    public String packHash() {
        return packHash;
    }

    public boolean isRequired() {
        return required;
    }
}
