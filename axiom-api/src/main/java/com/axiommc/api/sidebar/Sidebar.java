package com.axiommc.api.sidebar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

/**
 * A per-player sidebar displayed on the right side of the screen.
 *
 * <p>Sidebars display a title and up to 15 lines of text. Lines are indexed
 * from top to bottom, with index 0 being the topmost line. Updates are
 * immediately visible to all viewing players.
 */
public interface Sidebar {

    /**
     * Updates the sidebar title.
     *
     * <p>The title is displayed above all sidebar lines.
     * Visible immediately to all current viewers.
     *
     * @param title the new title
     */
    void title(ChatComponent title);

    /**
     * Sets a line at the given index.
     *
     * <p>Index 0 is the topmost line, index 14 is the bottommost.
     * Visible immediately to all current viewers.
     *
     * @param index the line index (0-14)
     * @param text the line content
     */
    void line(int index, ChatComponent text);

    /**
     * Removes a line from the sidebar.
     *
     * <p>Visible immediately to all current viewers.
     *
     * @param index the line index to remove
     */
    void removeLine(int index);

    /**
     * Shows this sidebar to a player.
     *
     * <p>Sends all current lines and the title to the player.
     *
     * @param player the player to show the sidebar to
     */
    void show(Player player);

    /**
     * Hides this sidebar from a player.
     *
     * @param player the player to hide the sidebar from
     */
    void hide(Player player);

    /**
     * Destroys this sidebar and releases resources.
     *
     * <p>Hides the sidebar from all viewing players and cleans up.
     */
    void destroy();
}
