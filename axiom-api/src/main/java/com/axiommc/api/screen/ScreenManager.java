package com.axiommc.api.screen;

import com.axiommc.api.player.Player;

import java.util.UUID;

/**
 * Opens, updates, and closes virtual screens for players.
 *
 * <p>Obtain an instance via your plugin's {@code ModuleContext}.
 */
public interface ScreenManager {

    /**
     * Opens the given screen for the player. If the player already has a
     * screen open, it is replaced.
     *
     * @param player the player to open the screen for
     * @param screen the screen to open
     * @return a session UUID identifying this screen instance for updates
     */
    UUID open(Player player, Screen screen);

    /**
     * Pushes an updated screen state to an already-open session.
     * If the session is no longer active, this is a no-op.
     *
     * @param sessionId the session UUID returned by {@link #open}
     * @param screen    the updated screen state
     */
    void update(UUID sessionId, Screen screen);

    /**
     * Closes the given session. Movement is restored to the player on the
     * next tick. If the session is not open, this is a no-op.
     *
     * @param sessionId the session UUID to close
     */
    void close(UUID sessionId);

    void close(Player player);

    boolean isOpen(Player player);

    /**
     * Returns whether the given session is currently open.
     */
    boolean isOpen(UUID sessionId);
}
