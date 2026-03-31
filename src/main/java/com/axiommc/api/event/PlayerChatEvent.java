package com.axiommc.api.event;

import com.axiommc.api.chat.SignedMessage;

/**
 * Fired when a player sends a chat message, before it is broadcast.
 * Cancel the event to prevent the message from reaching other players.
 */
public class PlayerChatEvent extends Event implements Cancellable {

    private final SignedMessage signedMessage;
    private boolean cancelled;

    public PlayerChatEvent(SignedMessage signedMessage) {
        this.signedMessage = signedMessage;
    }

    public SignedMessage signedMessage() { return signedMessage; }

    @Override public boolean isCancelled()           { return cancelled; }
    @Override public void    setCancelled(boolean c) { cancelled = c; }
}
