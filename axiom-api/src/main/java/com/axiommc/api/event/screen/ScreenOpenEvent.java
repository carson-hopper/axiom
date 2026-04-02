package com.axiommc.api.event.screen;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.Screen;
import java.util.Objects;
import java.util.UUID;

public final class ScreenOpenEvent extends Event {

    private final Player player;
    private final UUID sessionId;
    private final Screen screen;

    public ScreenOpenEvent(Player player, UUID sessionId, Screen screen) {
        this.player = Objects.requireNonNull(player, "player must not be null");
        this.sessionId = Objects.requireNonNull(sessionId, "sessionId must not be null");
        this.screen = Objects.requireNonNull(screen, "screen must not be null");
    }

    public Player player() {
        return player;
    }

    public UUID sessionId() {
        return sessionId;
    }

    public Screen screen() {
        return screen;
    }
}
