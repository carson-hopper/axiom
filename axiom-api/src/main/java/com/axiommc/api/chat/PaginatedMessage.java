package com.axiommc.api.chat;

import com.axiommc.api.command.CommandSender;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * Builds paginated chat output with clickable previous/next navigation.
 *
 * <pre>{@code
 * PaginatedMessage.builder()
 *     .title(ChatComponent.text("Help", ChatColor.GOLD).bold(true))
 *     .entries(entries)
 *     .entriesPerPage(8)
 *     .command("/help")
 *     .build()
 *     .send(sender, 1);
 * }</pre>
 */
public final class PaginatedMessage {

    private final ChatComponent title;
    private final List<ChatComponent> entries;
    private final int entriesPerPage;
    private final String command;

    private PaginatedMessage(
        ChatComponent title, List<ChatComponent> entries, int entriesPerPage, String command) {
        this.title = title;
        this.entries = List.copyOf(entries);
        this.entriesPerPage = entriesPerPage;
        this.command = command;
    }

    public int totalPages() {
        if (entries.isEmpty()) {
            return 1;
        }
        return (int) Math.ceil((double) entries.size() / entriesPerPage);
    }

    public void send(CommandSender sender, int page) {
        int total = totalPages();
        int clamped = Math.max(1, Math.min(page, total));

        sender.sendMessage(buildPage(clamped, total));
    }

    private ChatComponent buildPage(int page, int total) {
        int from = (page - 1) * entriesPerPage;
        int to = Math.min(from + entriesPerPage, entries.size());

        // Header line: ─── Title (page/total) ───

        ChatComponent result = buildHeader(page, total);
        for (int i = from; i < to; i++) {
            result = result.newLine().append(entries.get(i));
        }
        result = result.newLine().append(buildNavigation(page, total));
        return result;
    }

    private ChatComponent buildHeader(int page, int total) {
        ChatComponent divider = ChatComponent.text("───────── ", ChatColor.DARK_GRAY);
        ChatComponent titlePart =
            title != null ? title : ChatComponent.text("Page", ChatColor.WHITE);
        ChatComponent pageLabel =
            ChatComponent.text(" (" + page + "/" + total + ")", ChatColor.GRAY);
        ChatComponent dividerEnd = ChatComponent.text(" ─────────", ChatColor.DARK_GRAY);
        return divider.append(titlePart).append(pageLabel).append(dividerEnd);
    }

    private ChatComponent buildNavigation(int page, int total) {
        ChatComponent nav = ChatComponent.empty();

        if (page > 1) {
            nav = nav.append(ChatComponent.text("« Prev", ChatColor.AQUA)
                .clickRunCommand(command + " " + (page - 1))
                .hoverText(ChatComponent.text("Go to page " + (page - 1))));
        } else {
            nav = nav.append(ChatComponent.text("« Prev", ChatColor.DARK_GRAY));
        }

        nav = nav.append(ChatComponent.text("  |  ", ChatColor.DARK_GRAY));

        if (page < total) {
            nav = nav.append(ChatComponent.text("Next »", ChatColor.AQUA)
                .clickRunCommand(command + " " + (page + 1))
                .hoverText(ChatComponent.text("Go to page " + (page + 1))));
        } else {
            nav = nav.append(ChatComponent.text("Next »", ChatColor.DARK_GRAY));
        }

        return nav;
    }

    // ── Builder ────────────────────────────────────────────────────────────

    public static Builder builder() {
        return new Builder();
    }

    public static final class Builder {

        private ChatComponent title;
        private final List<ChatComponent> entries = new ArrayList<>();
        private int entriesPerPage = 10;
        private String command = "";

        private Builder() {}

        public Builder title(ChatComponent title) {
            this.title = title;
            return this;
        }

        public Builder entry(ChatComponent entry) {
            this.entries.add(Objects.requireNonNull(entry, "entry must not be null"));
            return this;
        }

        public Builder entries(List<ChatComponent> entries) {
            Objects.requireNonNull(entries, "entries must not be null");
            entries.forEach(
                e -> this.entries.add(Objects.requireNonNull(e, "entry must not be null")));
            return this;
        }

        public Builder entriesPerPage(int count) {
            if (count < 1) {
                throw new IllegalArgumentException("entriesPerPage must be >= 1");
            }
            this.entriesPerPage = count;
            return this;
        }

        /** Base command for navigation, e.g. "/help". Page number is appended automatically. */
        public Builder command(String command) {
            this.command = Objects.requireNonNull(command, "command must not be null");
            return this;
        }

        public PaginatedMessage build() {
            return new PaginatedMessage(title, entries, entriesPerPage, command);
        }
    }
}
