package com.axiommc.api.screen;

import com.axiommc.api.event.screen.ScreenClickEvent;

/**
 * Callback invoked when a player clicks an interactive {@link ScreenElement}.
 *
 * @see ScreenElement.Button
 * @see ScreenElement.ItemSlot
 */
@FunctionalInterface
public interface ScreenClickHandler {

    /**
     * Called on the server thread when the player clicks the element.
     *
     * @param event details about the click
     */
    void onClick(ScreenClickEvent event);
}