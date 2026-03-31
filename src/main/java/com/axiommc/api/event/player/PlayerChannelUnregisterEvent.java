package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player unregisters a plugin channel.
 */
public class PlayerChannelUnregisterEvent extends Event {

    private final Player player;
    private final String channel;

    public PlayerChannelUnregisterEvent(Player player, String channel) {
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
