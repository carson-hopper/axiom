package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player performs an animation (e.g. arm swing).
 */
public class PlayerAnimationEvent extends Event {

    private final Player player;
    private final String animationType;

    public PlayerAnimationEvent(Player player, String animationType) {
        this.player = player;
        this.animationType = animationType;
    }

    public Player player() {
        return player;
    }

    public String animationType() {
        return animationType;
    }
}
