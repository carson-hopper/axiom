package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/** Fired when a player sends updated movement input. */
public final class PlayerInputEvent extends Event {

    private final Player player;
    private final boolean forward;
    private final boolean backward;
    private final boolean left;
    private final boolean right;
    private final boolean jump;
    private final boolean shift;
    private final boolean sprint;

    public PlayerInputEvent(
        Player player,
        boolean forward,
        boolean backward,
        boolean left,
        boolean right,
        boolean jump,
        boolean shift,
        boolean sprint) {
        this.player = player;
        this.forward = forward;
        this.backward = backward;
        this.left = left;
        this.right = right;
        this.jump = jump;
        this.shift = shift;
        this.sprint = sprint;
    }

    public Player player() {
        return player;
    }

    public boolean forward() {
        return forward;
    }

    public boolean backward() {
        return backward;
    }

    public boolean left() {
        return left;
    }

    public boolean right() {
        return right;
    }

    public boolean jump() {
        return jump;
    }

    public boolean shift() {
        return shift;
    }

    public boolean sprint() {
        return sprint;
    }
}
