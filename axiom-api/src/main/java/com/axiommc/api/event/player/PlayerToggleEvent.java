package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player toggles movement states.
 *
 * <p>Subtypes: {@link Sneak}, {@link Sprint}.
 */
public class PlayerToggleEvent {

    private PlayerToggleEvent() {}

    /** Fired when a player toggles sneaking. */
    public static class Sneak extends Event {

        private final Player player;
        private final boolean sneaking;

        public Sneak(Player player, boolean sneaking) {
            this.player = player;
            this.sneaking = sneaking;
        }

        public Player player() {
            return player;
        }

        public boolean isSneaking() {
            return sneaking;
        }
    }

    /** Fired when a player toggles sprinting. */
    public static class Sprint extends Event {

        private final Player player;
        private final boolean sprinting;

        public Sprint(Player player, boolean sprinting) {
            this.player = player;
            this.sprinting = sprinting;
        }

        public Player player() {
            return player;
        }

        public boolean isSprinting() {
            return sprinting;
        }
    }
}
