package com.axiommc.api.event.gui;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

import java.util.Objects;
import java.util.UUID;

public final class GuiCloseEvent extends Event {

    private final Player player;
    private final UUID sessionId;

    public GuiCloseEvent(Player player, UUID sessionId) {
        this.player = Objects.requireNonNull(player, "player must not be null");
        this.sessionId = Objects.requireNonNull(sessionId, "sessionId must not be null");
    }

    public Player player()  { return player; }
    public UUID sessionId()      { return sessionId; }
}
