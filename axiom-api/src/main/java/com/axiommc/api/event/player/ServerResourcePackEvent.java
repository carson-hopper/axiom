package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import java.net.URI;

/**
 * Events fired during the server resource pack lifecycle.
 *
 * <ul>
 *   <li>{@link Send} — a resource pack is being sent to a player
 *   <li>{@link Request} — a resource pack is being requested for a player
 *   <li>{@link Remove} — a resource pack is being removed from a player
 * </ul>
 */
public final class ServerResourcePackEvent {

    private ServerResourcePackEvent() {}

    /**
     * Fired when a resource pack is sent to a player.
     */
    public static class Send extends Event {

        private final Player player;
        private final URI packUrl;

        public Send(Player player, URI packUrl) {
            this.player = player;
            this.packUrl = packUrl;
        }

        public Player player() {
            return player;
        }

        public URI packUrl() {
            return packUrl;
        }
    }

    /**
     * Fired when a resource pack is requested for a player.
     */
    public static class Request extends Event {

        private final Player player;
        private final URI packUrl;
        private final String packHash;
        private final boolean required;

        public Request(Player player, URI packUrl, String packHash, boolean required) {
            this.player = player;
            this.packUrl = packUrl;
            this.packHash = packHash;
            this.required = required;
        }

        public Player player() {
            return player;
        }

        public URI packUrl() {
            return packUrl;
        }

        public String packHash() {
            return packHash;
        }

        public boolean isRequired() {
            return required;
        }
    }

    /**
     * Fired when a resource pack is removed from a player.
     */
    public static class Remove extends Event {

        private final Player player;

        public Remove(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
        }
    }
}
