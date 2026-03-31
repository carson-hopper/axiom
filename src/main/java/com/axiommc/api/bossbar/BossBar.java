package com.axiommc.api.bossbar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

/**
 * A boss bar displayed at the top of the player's screen.
 *
 * <p>Boss bars show a health bar with a title and are commonly used for
 * boss fights or other significant events.
 */
public interface BossBar {

    /**
     * Sets the boss bar title.
     *
     * @param title the new title
     */
    void title(ChatComponent title);

    /**
     * Sets the progress (fill amount) of the boss bar.
     *
     * @param progress the progress from 0.0 (empty) to 1.0 (full)
     */
    void process(float progress);

    /**
     * Sets the color of the boss bar.
     *
     * @param color the bar color
     */
    void color(BossBarColor color);

    /**
     * Sets the style of the boss bar.
     *
     * @param style the bar style
     */
    void style(BossBarStyle style);

    /**
     * Shows the boss bar to a player.
     *
     * @param player the player to show the bar to
     */
    void addPlayer(Player player);

    /**
     * Hides the boss bar from a player.
     *
     * @param player the player to hide the bar from
     */
    void removePlayer(Player player);

    /**
     * Destroys this boss bar and removes it from all players.
     */
    void destroy();

    /**
     * Checks whether this boss bar is currently active.
     *
     * @return true if the boss bar is active, false if destroyed
     */
    boolean isActive();

    /**
     * Immutable specification for creating a boss bar.
     *
     * <p>Use {@link #of(ChatComponent)} or builder pattern methods to construct.
     *
     * @param title the boss bar title
     * @param progress the initial progress (0.0-1.0)
     * @param color the bar color
     * @param style the bar style
     */
    record Spec(ChatComponent title, float progress, BossBarColor color, BossBarStyle style) {

        /**
         * Creates a boss bar spec with a title and default settings.
         *
         * <p>Defaults: progress 1.0, color PURPLE, style PROGRESS.
         *
         * @param title the boss bar title
         * @return a new boss bar spec
         */
        public static Spec of(ChatComponent title) {
            return new Spec(title, 1.0f, BossBarColor.PURPLE, BossBarStyle.PROGRESS);
        }

        /**
         * Creates a new spec with the specified title.
         *
         * @param title the new title
         * @return a new spec with the updated title
         */
        public Spec title(ChatComponent title) {
            return new Spec(title, progress, color, style);
        }

        /**
         * Creates a new spec with the specified progress.
         *
         * @param progress the new progress
         * @return a new spec with the updated progress
         */
        public Spec progress(float progress) {
            return new Spec(title, progress, color, style);
        }

        /**
         * Creates a new spec with the specified color.
         *
         * @param color the new color
         * @return a new spec with the updated color
         */
        public Spec color(BossBarColor color) {
            return new Spec(title, progress, color, style);
        }

        /**
         * Creates a new spec with the specified style.
         *
         * @param style the new style
         * @return a new spec with the updated style
         */
        public Spec style(BossBarStyle style) {
            return new Spec(title, progress, color, style);
        }
    }
}
