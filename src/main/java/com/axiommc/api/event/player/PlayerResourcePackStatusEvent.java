package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Fired when a player responds to a resource pack request.
 */
public class PlayerResourcePackStatusEvent extends Event {

    public enum Status {
        ACCEPTED,
        DECLINED,
        FAILED,
        LOADED
    }

    private final Player player;
    private final Status status;

    public PlayerResourcePackStatusEvent(Player player, Status status) {
        this.player = player;
        this.status = status;
    }

    public Player player() {
        return player;
    }

    public Status status() {
        return status;
    }
}
