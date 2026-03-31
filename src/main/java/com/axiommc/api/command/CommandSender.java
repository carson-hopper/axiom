package com.axiommc.api.command;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

import java.util.Optional;

/**
 * Represents an entity that can send and receive commands.
 *
 * <p>A command sender can be a player, console, or other entity. This
 * interface provides methods to send messages and check permissions.
 */
public interface CommandSender {

    /**
     * Sends a plain text message to this command sender.
     *
     * @param message the message content (must not be null)
     */
    void sendMessage(String message);

    /**
     * Sends a formatted chat component message to this command sender.
     *
     * @param component the chat component (must not be null)
     */
    void sendMessage(ChatComponent component);

    /**
     * Sends multiple formatted chat component messages to this command sender.
     *
     * @param components the chat components to send (must not be null)
     */
    default void sendMessage(ChatComponent... components) {
        for (ChatComponent c : components) sendMessage(c);
    }

    /**
     * Checks whether this sender has permission to run a specific command.
     *
     * @param permission the permission string to check
     * @return true if this sender has the permission, false otherwise
     */
    boolean hasPermission(String permission);

    /**
     * Checks whether this command sender is a player.
     *
     * @return true if this sender is a player, false otherwise
     */
    boolean isPlayer();

    /**
     * Converts this command sender to a player if possible.
     *
     * @return an Optional containing the player if this sender is a player,
     *         otherwise an empty Optional
     */
    Optional<Player> asPlayer();

}
