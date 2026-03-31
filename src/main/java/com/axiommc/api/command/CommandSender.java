package com.axiommc.api.command;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

import java.util.Optional;

public interface CommandSender {

    void sendMessage(String message);
    void sendMessage(ChatComponent component);
    default void sendMessage(ChatComponent... components) {
        for (ChatComponent c : components) sendMessage(c);
    }

    boolean hasPermission(String permission);

    boolean isPlayer();
    Optional<Player> asPlayer();

}
