package com.axiommc.api.chat;

import com.axiommc.api.player.Player;
import java.time.Instant;

/**
 * A chat message that may carry a cryptographic signature.
 * The {@link #delete()} method broadcasts removal to all online players.
 * On Velocity, {@code delete()} is a no-op (removal is server-side only).
 */
public interface SignedMessage {

    Player sender();

    String content();

    Instant timestamp();

    boolean isSigned();

    void delete();
}
