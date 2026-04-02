package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player registers or unregisters a plugin channel.
 *
 * <ul>
 *   <li>{@link Register} — a plugin channel was registered
 *   <li>{@link Unregister} — a plugin channel was unregistered
 * </ul>
 */
public final class PlayerPluginChannelEvent {

    private PlayerPluginChannelEvent() {}

    /**
     * Fired when a player registers a plugin channel.
     */
    public static class Register extends Event {

        private final Player player;
        private final String channel;

        public Register(Player player, String channel) {
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

    /**
     * Fired when a player unregisters a plugin channel.
     */
    public static class Unregister extends Event {

        private final Player player;
        private final String channel;

        public Unregister(Player player, String channel) {
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
}
