package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player interacts with a bed.
 *
 * <p>Subtypes: {@link Enter}, {@link Leave}.
 */
public class PlayerBedEvent {

    private PlayerBedEvent() {}

    /** Fired when a player enters a bed. */
    public static class Enter extends Event implements Cancellable {

        private final Player player;
        private final Location bedLocation;
        private boolean cancelled = false;

        public Enter(Player player, Location bedLocation) {
            this.player = player;
            this.bedLocation = bedLocation;
        }

        public Player player() {
            return player;
        }

        public Location bedLocation() {
            return bedLocation;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /** Fired when a player leaves a bed. */
    public static class Leave extends Event {

        private final Player player;
        private final Location bedLocation;

        public Leave(Player player, Location bedLocation) {
            this.player = player;
            this.bedLocation = bedLocation;
        }

        public Player player() {
            return player;
        }

        public Location bedLocation() {
            return bedLocation;
        }
    }
}
