package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;

/**
 * Events fired when a player's experience or level changes.
 *
 * <p>Subtypes: {@link Change}, {@link LevelChange}.
 */
public class PlayerExperienceEvent {

    private PlayerExperienceEvent() {}

    /** Fired when a player's experience changes. */
    public static class Change extends Event implements Cancellable {

        private final Player player;
        private final int amount;
        private boolean cancelled = false;

        public Change(Player player, int amount) {
            this.player = player;
            this.amount = amount;
        }

        public Player player() {
            return player;
        }

        public int amount() {
            return amount;
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

    /** Fired when a player's level changes. */
    public static class LevelChange extends Event {

        private final Player player;
        private final int oldLevel;
        private final int newLevel;

        public LevelChange(Player player, int oldLevel, int newLevel) {
            this.player = player;
            this.oldLevel = oldLevel;
            this.newLevel = newLevel;
        }

        public Player player() {
            return player;
        }

        public int oldLevel() {
            return oldLevel;
        }

        public int newLevel() {
            return newLevel;
        }
    }
}
