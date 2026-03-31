package com.axiommc.api.gui;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

import java.util.Arrays;
import java.util.Objects;
import java.util.UUID;
import java.util.function.Consumer;

/**
 * An immutable chest GUI definition. Use {@link #builder()} to construct,
 * or {@link #toBuilder()} to derive a modified copy.
 *
 * <p>For a GUI that changes over time, rebuild from {@code toBuilder()} and
 * call {@link GuiManager#update(UUID, Gui)} to push the new state.
 */
public record Gui(ChatComponent title, GuiSize size, GuiItem[] slots, Consumer<Player> closeHandler) {

    public Gui(ChatComponent title, GuiSize size, GuiItem[] slots, Consumer<Player> closeHandler) {
        this.title = title;
        this.size = size;
        this.slots = Arrays.copyOf(slots, slots.length);
        this.closeHandler = closeHandler;
    }

    /**
     * Returns the item in the given slot, or {@code null} if the slot is empty.
     */
    public GuiItem getSlot(int slot) {
        if (slot < 0 || slot >= slots.length) return null;
        return slots[slot];
    }

    @Override
    public GuiItem[] slots() {
        return Arrays.copyOf(slots, slots.length);
    }

    /**
     * Fires the close handler if one is registered.
     */
    public void handleClose(Player player) {
        if (closeHandler != null) closeHandler.accept(player);
    }

    /**
     * Returns a builder pre-populated with this GUI's current state.
     */
    public Builder toBuilder() {
        Builder b = new Builder(size).title(title);
        b.closeHandler = closeHandler;
        for (int i = 0; i < slots.length; i++) {
            b.slots[i] = slots[i];
        }
        return b;
    }

    public static Builder builder(GuiSize size) {
        return new Builder(size);
    }

    // ── Builder ────────────────────────────────────────────────────────────

    public static final class Builder {

        private ChatComponent title = ChatComponent.text("GUI");
        private final GuiSize size;
        private final GuiItem[] slots;
        private Consumer<Player> closeHandler;

        private Builder(GuiSize size) {
            this.size = Objects.requireNonNull(size, "size must not be null");
            this.slots = new GuiItem[size.slots()];
        }

        public Builder title(ChatComponent title) {
            this.title = Objects.requireNonNull(title, "title must not be null");
            return this;
        }

        public Builder title(String title) {
            this.title = ChatComponent.text(title);
            return this;
        }

        /**
         * Place an item in the given slot (0-indexed).
         */
        public Builder slot(int index, GuiItem item) {
            if (index < 0 || index >= slots.length) {
                throw new IndexOutOfBoundsException("Slot " + index + " out of range for size " + size);
            }
            this.slots[index] = item;
            return this;
        }

        /**
         * Fill all slots with the given item.
         */
        public Builder fill(GuiItem item) {
            Arrays.fill(slots, Objects.requireNonNull(item, "item must not be null"));
            return this;
        }

        /**
         * Fill the border (first/last row + first/last column) with the given item.
         */
        public Builder border(GuiItem item) {
            Objects.requireNonNull(item, "item must not be null");
            int rows = size.rows();
            for (int col = 0; col < 9; col++) {
                slots[col] = item;                         // top row
                slots[(rows - 1) * 9 + col] = item;       // bottom row
            }
            for (int row = 1; row < rows - 1; row++) {
                slots[row * 9] = item;                     // left column
                slots[row * 9 + 8] = item;                 // right column
            }
            return this;
        }

        /**
         * Called when the player closes this GUI.
         */
        public Builder onClose(Consumer<Player> handler) {
            this.closeHandler = Objects.requireNonNull(handler, "handler must not be null");
            return this;
        }

        public Gui build() {
            return new Gui(title, size, slots, closeHandler);
        }
    }
}
