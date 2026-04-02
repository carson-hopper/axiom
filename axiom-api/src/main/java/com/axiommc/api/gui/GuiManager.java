package com.axiommc.api.gui;

import com.axiommc.api.player.Player;
import java.util.UUID;

/**
 * Opens, updates, and closes GUIs for players.
 *
 * <p>Obtained via {@link dev.helix.api.module.ModuleContext#getGuiManager()}.
 */
public interface GuiManager {

    /**
     * Opens the given GUI for the player. If the player already has a GUI open,
     * it is replaced.
     *
     * @param player the player to open the GUI for
     * @param gui    the GUI to open
     * @return a session UUID identifying this GUI instance for subsequent updates
     */
    UUID open(Player player, Gui gui);

    /**
     * Pushes a new GUI state to an already-open GUI session. If the session is
     * no longer open (player closed it), this is a no-op.
     *
     * @param sessionId the session UUID returned by {@link #open}
     * @param gui       the updated GUI state
     */
    void update(UUID sessionId, Gui gui);

    /**
     * Closes the GUI session. The player's inventory is restored to their normal
     * inventory on the next tick. If the session is not open, this is a no-op.
     *
     * @param sessionId the session UUID to close
     */
    void close(UUID sessionId);

    /**
     * Returns whether the given session is currently open.
     */
    boolean isOpen(UUID sessionId);
}
