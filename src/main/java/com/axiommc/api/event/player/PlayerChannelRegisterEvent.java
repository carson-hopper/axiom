package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player registers a plugin channel.
 */
public class PlayerChannelRegisterEvent extends Event {

    private final Player player;
    private final String channel;

    public PlayerChannelRegisterEvent(Player player, String channel) {
        this.player = player;
        this.channel = channel;
    }

    public Player player() {
        return player;
    }

    public String channel() {
        return channel;
    }
}
