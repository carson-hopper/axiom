package com.axiommc.api.screen;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.event.screen.ScreenClickEvent;

/**
 * A single element within a {@link Screen}.
 *
 * <p>Elements are positioned in normalized screen space: (0, 0) = top-left,
 * (1, 1) = bottom-right. All factory methods and subtypes are declared here.
 *
 * <h2>Element types</h2>
 * <ul>
 *   <li>{@link Panel}  — background block_display rectangle
 *   <li>{@link Label}  — text_display text element
 *   <li>{@link Button} — text_display + interaction entity with click callback
 *   <li>{@link ItemSlot} — item_display with optional click callback
 * </ul>
 */
public sealed interface ScreenElement
        permits ScreenElement.Panel,
        ScreenElement.Label,
        ScreenElement.Button,
        ScreenElement.ItemSlot {

    // ── Panel ─────────────────────────────────────────────────────────────────

    /**
     * A solid background panel rendered as a scaled {@code block_display}.
     * Non-interactive; use it to frame groups of other elements.
     *
     * @param x     left edge (0..1)
     * @param y     top edge (0..1)
     * @param width width (0..1)
     * @param height height (0..1)
     * @param style visual style of the panel
     */
    record Panel(float x, float y, float width, float height, PanelStyle style)
            implements ScreenElement {

        public static Panel of(float x, float y, float width, float height) {
            return new Panel(x, y, width, height, PanelStyle.DARK);
        }

        public static Panel of(float x, float y, float width, float height, PanelStyle style) {
            return new Panel(x, y, width, height, style);
        }
    }

    // ── Label ─────────────────────────────────────────────────────────────────

    /**
     * A text label rendered as a {@code text_display} entity.
     * Non-interactive.
     *
     * @param x    center-x position (0..1)
     * @param y    center-y position (0..1)
     * @param text the component to display
     */
    record Label(float x, float y, ChatComponent text) implements ScreenElement {

        public static Label of(float x, float y, ChatComponent text) {
            return new Label(x, y, text);
        }

        public static Label of(float x, float y, String text) {
            return new Label(x, y, ChatComponent.text(text));
        }
    }

    // ── Button ────────────────────────────────────────────────────────────────

    /**
     * A clickable button backed by a {@code text_display} + {@code interaction} entity pair.
     *
     * <p>Hover state is handled automatically — the label color shifts on hover.
     * The {@code onClick} callback runs on the server thread when the player
     * left-clicks the button.
     *
     * @param x       left edge (0..1)
     * @param y       top edge (0..1)
     * @param width   width (0..1)
     * @param height  height (0..1)
     * @param label   the button label component
     * @param onClick callback fired on click
     */
    record Button(float x, float y, float width, float height,
                  ChatComponent label,
                  ScreenClickHandler onClick)
            implements ScreenElement {

        public static Button of(float x, float y, float width, float height,
                                ChatComponent label, ScreenClickHandler onClick) {
            return new Button(x, y, width, height, label, onClick);
        }

        public static Button of(float x, float y, float width, float height,
                                String label, ScreenClickHandler onClick) {
            return new Button(x, y, width, height, ChatComponent.text(label), onClick);
        }
    }

    // ── ItemSlot ──────────────────────────────────────────────────────────────

    /**
     * An item rendered as an {@code item_display} entity.
     *
     * <p>If {@code onClick} is non-null, an invisible {@code interaction} entity
     * is co-spawned as a hitbox and the callback fires on click.
     *
     * @param x       center-x (0..1)
     * @param y       center-y (0..1)
     * @param size    uniform size in screen-space
     * @param item    the item to display (material key, e.g. {@code "minecraft:diamond"})
     * @param onClick click callback, or null for a non-interactive display
     */
    record ItemSlot(float x, float y, float size, String item,
                    ScreenClickHandler onClick)
            implements ScreenElement {

        public static ItemSlot display(float x, float y, float size, String item) {
            return new ItemSlot(x, y, size, item, null);
        }

        public static ItemSlot of(float x, float y, float size, String item,
                                  ScreenClickHandler onClick) {
            return new ItemSlot(x, y, size, item, onClick);
        }
    }
}