package com.axiommc.api.gui;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.chat.Item;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * A multi-page {@link Gui} that automatically renders prev/next navigation buttons.
 *
 * <p>Content items fill all non-navigation slots. The last row is reserved for
 * navigation: slot 45 = previous page, slot 53 = next page (requires SIX_ROWS).
 * For smaller GUIs the content area is reduced by one row.
 */
public final class PagedGui {

    private final ChatComponent title;
    private final GuiSize size;
    private final List<GuiItem> contentItems;
    private final GuiItem prevButton;
    private final GuiItem nextButton;
    private final GuiItem emptyFill;

    private PagedGui(ChatComponent title, GuiSize size, List<GuiItem> contentItems,
                     GuiItem prevButton, GuiItem nextButton, GuiItem emptyFill) {
        this.title = title;
        this.size = size;
        this.contentItems = List.copyOf(contentItems);
        this.prevButton = prevButton;
        this.nextButton = nextButton;
        this.emptyFill = emptyFill;
    }

    public int contentSlotsPerPage() {
        // Last row reserved for navigation
        return size.slots() - 9;
    }

    public int totalPages() {
        if (contentItems.isEmpty()) return 1;
        return (int) Math.ceil((double) contentItems.size() / contentSlotsPerPage());
    }

    /**
     * Builds and returns the {@link Gui} for the given page (1-indexed).
     * Page is clamped to [1, totalPages()].
     */
    public Gui buildPage(int page, GuiManager guiManager) {
        int total = totalPages();
        int clamped = Math.max(1, Math.min(page, total));
        int perPage = contentSlotsPerPage();
        int from = (clamped - 1) * perPage;

        Gui.Builder builder = Gui.builder(size)
            .title(ChatComponent.text("")
                .append(title)
                .append(ChatComponent.text(" (" + clamped + "/" + total + ")", ChatColor.GRAY)));

        // Fill content slots
        for (int i = 0; i < perPage; i++) {
            int itemIndex = from + i;
            if (itemIndex < contentItems.size()) {
                builder.slot(i, contentItems.get(itemIndex));
            } else if (emptyFill != null) {
                builder.slot(i, emptyFill);
            }
        }

        // Navigation row (last row)
        int navOffset = size.slots() - 9;

        if (clamped > 1 && prevButton != null) {
            int prevPage = clamped - 1;
            builder.slot(navOffset, prevButton.withClickHandler(e ->
                guiManager.open(e.player(), buildPage(prevPage, guiManager))));
        }

        if (clamped < total && nextButton != null) {
            int nextPage = clamped + 1;
            builder.slot(navOffset + 8, nextButton.withClickHandler(e ->
                guiManager.open(e.player(), buildPage(nextPage, guiManager))));
        }

        return builder.build();
    }

    // ── Builder ────────────────────────────────────────────────────────────

    public static Builder builder(GuiSize size) {
        return new Builder(size);
    }

    public static final class Builder {

        private final GuiSize size;
        private ChatComponent title = ChatComponent.text("Menu");
        private final List<GuiItem> contentItems = new ArrayList<>();
        private GuiItem prevButton;
        private GuiItem nextButton;
        private GuiItem emptyFill;

        private Builder(GuiSize size) {
            this.size = Objects.requireNonNull(size, "size must not be null");
        }

        public Builder title(ChatComponent title) {
            this.title = Objects.requireNonNull(title, "title must not be null");
            return this;
        }

        public Builder title(String title) {
            this.title = ChatComponent.text(title);
            return this;
        }

        public Builder item(GuiItem item) {
            this.contentItems.add(Objects.requireNonNull(item, "item must not be null"));
            return this;
        }

        public Builder items(List<GuiItem> items) {
            Objects.requireNonNull(items, "items must not be null");
            items.forEach(i -> this.contentItems.add(Objects.requireNonNull(i)));
            return this;
        }

        /** Item shown in the bottom-left slot when a previous page exists. */
        public Builder prevButton(Item item) {
            this.prevButton = GuiItem.of(Objects.requireNonNull(item));
            return this;
        }

        /** Item shown in the bottom-right slot when a next page exists. */
        public Builder nextButton(Item item) {
            this.nextButton = GuiItem.of(Objects.requireNonNull(item));
            return this;
        }

        /** Fills empty content slots with this item. */
        public Builder emptyFill(Item item) {
            this.emptyFill = GuiItem.of(Objects.requireNonNull(item));
            return this;
        }

        public PagedGui build() {
            return new PagedGui(title, size, contentItems, prevButton, nextButton, emptyFill);
        }
    }
}
