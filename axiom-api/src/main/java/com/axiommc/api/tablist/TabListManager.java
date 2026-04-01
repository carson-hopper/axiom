package com.axiommc.api.tablist;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

/**
 * Controls the player list (tab list) shown on the client's HUD.
 *
 * <h3>Header / footer</h3>
 * <p>Text displayed above and below the list of players. Each viewer has an
 * independent header/footer — changing one player's does not affect others.</p>
 *
 * <h3>Display names</h3>
 * <p>Each entry in the tab list shows a player's name. You can override that
 * name with a custom {@link ChatComponent} (e.g. to prepend a rank prefix).
 * The override is broadcast to all currently-online players and must be
 * re-applied when new players join.</p>
 */
public interface TabListManager {

    /** Sets the header and footer shown to {@code viewer}. */
    void headerAndFooter(Player viewer, ChatComponent header, ChatComponent footer);

    /** Clears the header and footer for {@code viewer}. */
    void clearHeaderAndFooter(Player viewer);

    /**
     * Overrides the display name shown for {@code target} in all currently-online
     * players' tab lists. Pass {@code null} to reset to the player's real name.
     */
    void displayName(Player target, ChatComponent displayName);

    /** Resets the display name for {@code target} to their real username. */
    default void resetDisplayName(Player target) {
        displayName(target, null);
    }
}
