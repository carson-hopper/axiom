package com.axiommc.api.screen;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.function.Consumer;

/**
 * An immutable virtual screen definition.
 *
 * <p>A Screen is a full-view UI rendered using fake display entities and
 * interaction entities — no client mods required. The player's movement is
 * locked while the screen is open; head rotation drives the cursor.
 *
 * <p>Use {@link #builder()} to construct a screen, or {@link #toBuilder()}
 * to derive a modified copy from an existing screen.
 *
 * <p>To show a screen to a player, use {@link ScreenManager#open(Player, Screen)}.
 *
 * @param title       title shown in the cursor overlay (used for accessibility)
 * @param elements    the UI elements that compose this screen
 * @param width       virtual plane width in world-units (default 5.0)
 * @param height      virtual plane height in world-units (default 3.5)
 * @param distance    distance in blocks in front of the player (default 2.5)
 * @param closeHandler callback invoked when the player closes the screen, or null
 */
public record Screen(
    ChatComponent title,
    List<ScreenElement> elements,
    float width,
    float height,
    float distance,
    Consumer<Player> closeHandler
) {

    public Screen(ChatComponent title, List<ScreenElement> elements,
                  float width, float height, float distance,
                  Consumer<Player> closeHandler) {
        this.title = Objects.requireNonNull(title, "title must not be null");
        this.elements = List.copyOf(elements);
        this.width = width;
        this.height = height;
        this.distance = distance;
        this.closeHandler = closeHandler;
    }

    /**
     * Invokes the close handler for this screen.
     *
     * @param player the player who closed the screen
     */
    public void handleClose(Player player) {
        if (closeHandler != null) {
            closeHandler.accept(player);
        }
    }

    /**
     * Creates a builder pre-populated with this screen's current state.
     *
     * @return a builder initialized with this screen's data
     */
    public Builder toBuilder() {
        Builder b = new Builder().title(title).width(width).height(height).distance(distance);
        b.elements.addAll(elements);
        b.closeHandler = closeHandler;
        return b;
    }

    /**
     * Creates a new screen builder with default dimensions.
     *
     * @return a new builder
     */
    public static Builder builder() {
        return new Builder();
    }

    // ── Builder ───────────────────────────────────────────────────────────────

    public static final class Builder {

        private ChatComponent title = ChatComponent.text("Screen");
        private final List<ScreenElement> elements = new ArrayList<>();
        private float width = 5.0f;
        private float height = 3.5f;
        private float distance = 2.5f;
        private Consumer<Player> closeHandler;

        private Builder() {}

        public Builder title(ChatComponent title) {
            this.title = Objects.requireNonNull(title, "title must not be null");
            return this;
        }

        public Builder title(String title) {
            this.title = ChatComponent.text(title);
            return this;
        }

        /**
         * Virtual plane width in world-units.
         */
        public Builder width(float width) {
            this.width = width;
            return this;
        }

        /**
         * Virtual plane height in world-units.
         */
        public Builder height(float height) {
            this.height = height;
            return this;
        }

        /**
         * Distance in blocks the screen plane sits in front of the player.
         */
        public Builder distance(float distance) {
            this.distance = distance;
            return this;
        }

        /**
         * Add a single element to this screen.
         */
        public Builder element(ScreenElement element) {
            this.elements.add(Objects.requireNonNull(element, "element must not be null"));
            return this;
        }

        /**
         * Add multiple elements to this screen.
         */
        public Builder elements(ScreenElement... elements) {
            this.elements.addAll(Arrays.asList(elements));
            return this;
        }

        /**
         * Add all elements from a list.
         */
        public Builder elements(List<ScreenElement> elements) {
            this.elements.addAll(elements);
            return this;
        }

        /**
         * Called when the player closes this screen (sneak key or explicit close).
         */
        public Builder onClose(Consumer<Player> handler) {
            this.closeHandler = Objects.requireNonNull(handler, "handler must not be null");
            return this;
        }

        public Screen build() {
            return new Screen(title, elements, width, height, distance, closeHandler);
        }
    }
}
