package com.axiommc.api.gui;

import com.axiommc.api.chat.Item;

import java.util.Objects;
import java.util.function.Consumer;

/**
 * A single slot in a GUI, combining an item display with an optional click handler.
 */
public record GuiItem(Item item, Consumer<GuiClickEvent> clickHandler) {

    public static GuiItem of(Item item) {
        return new GuiItem(Objects.requireNonNull(item, "item must not be null"), null);
    }

    public static GuiItem of(Item item, Consumer<GuiClickEvent> clickHandler) {
        return new GuiItem(
                Objects.requireNonNull(item, "item must not be null"),
                Objects.requireNonNull(clickHandler, "clickHandler must not be null")
        );
    }

    /**
     * Returns this item with a different click handler (or null to remove).
     */
    public GuiItem withClickHandler(Consumer<GuiClickEvent> handler) {
        return new GuiItem(item, handler);
    }

    public boolean hasClickHandler() {
        return clickHandler != null;
    }

    /**
     * Fires the click handler if one is registered.
     */
    public void handleClick(GuiClickEvent event) {
        if (clickHandler != null) {
            clickHandler.accept(event);
        }
    }
}
