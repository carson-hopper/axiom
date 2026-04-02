package com.axiommc.api.event.player;

import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player dies or respawns.
 *
 * <p>Subtypes: {@link Death}, {@link Respawn}.
 */
public class PlayerDeathEvent {

    private PlayerDeathEvent() {}

    /** Fired when a player dies. */
    public static class Death extends Event {

        private final Player player;
        private final String deathMessage;

        public Death(Player player, String deathMessage) {
            this.player = player;
            this.deathMessage = deathMessage;
        }

        public Player player() {
            return player;
        }

        public String deathMessage() {
            return deathMessage;
        }
    }

    /** Fired when a player respawns. */
    public static class Respawn extends Event {

        private final Player player;

        public Respawn(Player player) {
            this.player = player;
        }

        public Player player() {
            return player;
        }
    }
}
