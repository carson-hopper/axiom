package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player's client settings change.
 */
public class PlayerSettingsChangedEvent extends Event {

    private final Player player;
    private final String language;
    private final int viewDistance;
    private final boolean showCape;

    public PlayerSettingsChangedEvent(
        Player player, String language, int viewDistance, boolean showCape) {
        this.player = player;
        this.language = language;
        this.viewDistance = viewDistance;
        this.showCape = showCape;
    }

    public Player player() {
        return player;
    }

    public String language() {
        return language;
    }

    public int viewDistance() {
        return viewDistance;
    }

    public boolean isShowingCape() {
        return showCape;
    }
}
