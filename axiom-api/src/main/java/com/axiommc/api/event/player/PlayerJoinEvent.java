package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.Server;

import java.util.UUID;

/**
 * Events fired during the player join lifecycle.
 *
 * <ul>
 *   <li>{@link Init} — during login handshake, before authentication
 *   <li>{@link Pre} — before the player fully connects; cancellable
 *   <li>{@link Connecting} — the player is connecting to the server
 *   <li>{@link Post} — after the player has fully connected
 * </ul>
 */
public final class PlayerJoinEvent {

    private PlayerJoinEvent() {}

    /**
     * Fired during the login handshake when the server receives the
     * {@code ServerboundHelloPacket} (game profile request). This is
     * the earliest point in the join lifecycle — the player is not
     * yet authenticated or fully connected.
     *
     * <p>Primarily useful for offline-mode servers or proxy setups
     * where the player's UUID needs to be set or overridden before
     * authentication occurs. On standard online-mode servers this
     * event can generally be ignored.
     */
    public static class Init extends Event {

        private final String playerName;
        private UUID playerUUID;

        public Init(String playerName) {
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

    /**
     * Fired before a player connects. Cancelling this event
     * prevents the player from joining.
     */
    public static class Pre extends Event implements Cancellable {

        private final Player player;
        private Server targetServer;
        private boolean cancelled = false;

        public Pre(Player player, Server targetServer) {
            this.player = player;
            this.targetServer = targetServer;
        }

        public Player player() {
            return player;
        }

        public Server targetServer() {
            return targetServer;
        }

        public void targetServer(Server targetServer) {
            this.targetServer = targetServer;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when the player is actively connecting to the server.
     */
    public static class Connecting extends Event {

        private final Player player;

        public Connecting(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
        }
    }

    /**
     * Fired after the player has fully connected to the server.
     */
    public static class Post extends Event {

        private final Player player;
        private final Server server;

        public Post(Player player, Server server) {
            this.player = player;
            this.server = server;
        }

        public Player player() {
            return player;
        }

        public Server server() {
            return server;
        }
    }
}
