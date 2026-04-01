package com.axiommc.api.event.screen;

import com.axiommc.api.gui.ClickType;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.ScreenClickHandler;
import com.axiommc.api.screen.ScreenElement;

import java.util.Objects;

/**
 * Passed to a {@link ScreenClickHandler} when a player clicks an interactive
 * {@link ScreenElement} on a virtual screen.
 *
 * @param player    the player who clicked
 * @param cursor    cursor position in normalized screen space (0..1, 0..1)
 * @param clickType the type of click
 */
public record ScreenClickEvent(Player player, Vector2 cursor, ClickType clickType) {

    public ScreenClickEvent(Player player, Vector2 cursor, ClickType clickType) {
        this.player    = Objects.requireNonNull(player,    "player must not be null");
        this.cursor    = Objects.requireNonNull(cursor,    "cursor must not be null");
        this.clickType = Objects.requireNonNull(clickType, "clickType must not be null");
    }
}
