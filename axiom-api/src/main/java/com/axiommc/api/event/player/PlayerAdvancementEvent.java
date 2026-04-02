package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player completes an advancement.
 */
public class PlayerAdvancementEvent extends Event {

    private final Player player;
    private final String advancement;

    public PlayerAdvancementEvent(Player player, String advancement) {
        this.player = player;
        this.advancement = advancement;
    }

    public Player player() {
        return player;
    }

    public String advancement() {
        return advancement;
    }
}
