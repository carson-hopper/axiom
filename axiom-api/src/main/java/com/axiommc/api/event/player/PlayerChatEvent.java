package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player sends a chat message.
 *
 * <ul>
 *   <li>{@link Pre} — before the message is broadcast; cancellable
 *   <li>{@link Post} — after the message has been broadcast
 * </ul>
 */
public final class PlayerChatEvent {

    private PlayerChatEvent() {}

    /**
     * Fired before a chat message is broadcast. Cancel to suppress it.
     */
    public static class Pre extends Event implements Cancellable {

        private final Player player;
        private String message;
        private boolean cancelled = false;

        public Pre(Player player, String message) {
            this.player = player;
            this.message = message;
        }

        public Player player() {
            return player;
        }

        public String message() {
            return message;
        }

        public void message(String message) {
            this.message = message;
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
     * Fired after a chat message has been broadcast.
     */
    public static class Post extends Event {

        private final Player player;
        private final String message;

        public Post(Player player, String message) {
            this.player = player;
            this.message = message;
        }

        public Player player() {
            return player;
        }

        public String message() {
            return message;
        }
    }
}
