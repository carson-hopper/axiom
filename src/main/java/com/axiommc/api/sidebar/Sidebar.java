package com.axiommc.api.sidebar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

/**
 * A per-player sidebar (scoreboard sidebar slot) displayed on the right side of the screen.
 *
 * <p>Lines are indexed from the top: index 0 is the topmost line, higher indices appear lower.
 * Up to 15 lines are supported.
 */
public interface Sidebar {

    /** Updates the sidebar title shown above the lines. Visible immediately to all current viewers. */
    void title(ChatComponent title);

    /**
     * Sets a line at the given index (0 = top, 14 = bottom).
     * Visible immediately to all current viewers.
     */
    void line(int index, ChatComponent text);

    /** Removes the line at the given index. Visible immediately to all current viewers. */
    void removeLine(int index);

    /** Shows this sidebar to the player. Sends all current lines. */
    void show(Player player);

    /** Hides this sidebar from the player. */
    void hide(Player player);

    /** Hides the sidebar from all current viewers and releases resources. */
    void destroy();
}
