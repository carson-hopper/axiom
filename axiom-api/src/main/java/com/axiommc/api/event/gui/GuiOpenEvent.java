package com.axiommc.api.event.gui;

import com.axiommc.api.event.Event;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.player.Player;

import java.util.Objects;
import java.util.UUID;

public final class GuiOpenEvent extends Event {

    private final Player player;
    private final UUID sessionId;
    private final Gui gui;

    public GuiOpenEvent(Player player, UUID sessionId, Gui gui) {
        this.player = Objects.requireNonNull(player, "player must not be null");
        this.sessionId = Objects.requireNonNull(sessionId, "sessionId must not be null");
        this.gui = Objects.requireNonNull(gui, "gui must not be null");
    }

    public Player player() {
        return player;
    }

    public UUID sessionId() {
        return sessionId;
    }

    public Gui gui() {
        return gui; }

}
