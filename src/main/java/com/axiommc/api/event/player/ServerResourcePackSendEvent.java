package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a resource pack is sent to a player.
 */
public class ServerResourcePackSendEvent extends Event {

    private final Player player;
    private final String packUrl;

    public ServerResourcePackSendEvent(Player player, String packUrl) {
        this.player = player;
        this.packUrl = packUrl;
    }

    public Player player() {
        return player;
    }

    public String packUrl() {
        return packUrl;
    }
}
