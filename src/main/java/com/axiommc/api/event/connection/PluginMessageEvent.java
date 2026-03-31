package com.axiommc.api.event.connection;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a plugin message is received from a player.
 */
public class PluginMessageEvent extends Event {

    private final Player player;
    private final String channel;
    private final byte[] data;

    public PluginMessageEvent(Player player, String channel, byte[] data) {
        this.player = player;
        this.channel = channel;
        this.data = data;
    }

    public Player player() {
        return player;
    }

    public String channel() {
        return channel;
    }

    public byte[] data() {
        return data;
    }
}
