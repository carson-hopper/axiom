package com.axiommc.api.chat;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.UUID;

public record ChatComponent(String type, String content, String translatableKey, ChatComponent[] translatableArgs,
                            String keybind, ChatColor color, boolean bold, boolean italic, boolean underlined,
                            boolean strikethrough, boolean obfuscated, String insertion, ChatClickEvent clickEvent,
                            ChatHoverEvent hoverEvent, List<ChatComponent> children) {

    // Type constants
    private static final String TYPE_TEXT = "text";
    private static final String TYPE_TRANSLATABLE = "translatable";
    private static final String TYPE_KEYBIND = "keybind";
    private static final String TYPE_NEWLINE = "newline";
    private static final String TYPE_SPACE = "space";

    public ChatComponent(String type, String content, String translatableKey,
                         ChatComponent[] translatableArgs, String keybind,
                         ChatColor color, boolean bold, boolean italic, boolean underlined,
                         boolean strikethrough, boolean obfuscated, String insertion,
                         ChatClickEvent clickEvent, ChatHoverEvent hoverEvent,
                         List<ChatComponent> children) {
        this.type = type;
        this.content = content;
        this.translatableKey = translatableKey;
        this.translatableArgs = translatableArgs != null
            ? translatableArgs.clone()
            : new ChatComponent[0];
        this.keybind = keybind;
        this.color = color;
        this.bold = bold;
        this.italic = italic;
        this.underlined = underlined;
        this.strikethrough = strikethrough;
        this.obfuscated = obfuscated;
        this.insertion = insertion;
        this.clickEvent = clickEvent;
        this.hoverEvent = hoverEvent;
        this.children = List.copyOf(children);
    }

    // ── Internal copy-with ─────────────────────────────────────────────────

    private BuilderImpl copyToBuilder() {
        BuilderImpl b = new BuilderImpl(type, content, translatableKey, translatableArgs, keybind);
        b.color = this.color;
        b.bold = this.bold;
        b.italic = this.italic;
        b.underlined = this.underlined;
        b.strikethrough = this.strikethrough;
        b.obfuscated = this.obfuscated;
        b.insertion = this.insertion;
        b.clickEvent = this.clickEvent;
        b.hoverEvent = this.hoverEvent;
        b.children.addAll(this.children);
        return b;
    }

    // ── Static factories ───────────────────────────────────────────────────

    public static ChatComponent text(String content) {
        if (content == null) throw new NullPointerException("content must not be null");
        return new BuilderImpl(TYPE_TEXT, content, null, null, null).build();
    }

    public static ChatComponent text(String content, ChatColor color) {
        return text(content).color(color);
    }

    public static ChatComponent text(String content, ChatColor color, ChatDecoration... decorations) {
        ChatComponent c = text(content).color(color);
        for (ChatDecoration d : decorations) c = c.decoration(d, true);
        return c;
    }

    public static ChatComponent translatable(String key, ChatComponent... args) {
        return new BuilderImpl(TYPE_TRANSLATABLE, "", key, args, null).build();
    }

    public static ChatComponent keybind(String keybind) {
        // Use keybind value as content so toPlainText() returns the keybind string
        return new BuilderImpl(TYPE_KEYBIND, keybind, null, null, keybind).build();
    }

    public static ChatComponent empty() {
        return text("");
    }

    public static ChatComponent newline() {
        return new BuilderImpl(TYPE_NEWLINE, "\n", null, null, null).build();
    }

    private static ChatComponent spaceComponent() {
        return new BuilderImpl(TYPE_SPACE, " ", null, null, null).build();
    }

    // ── Styling (immutable) ────────────────────────────────────────────────

    public ChatComponent color(ChatColor color) {
        BuilderImpl b = copyToBuilder();
        b.color = color;
        return b.build();
    }

    public ChatComponent decoration(ChatDecoration decoration, boolean active) {
        BuilderImpl b = copyToBuilder();
        switch (decoration) {
            case BOLD -> b.bold = active;
            case ITALIC -> b.italic = active;
            case UNDERLINED -> b.underlined = active;
            case STRIKETHROUGH -> b.strikethrough = active;
            case OBFUSCATED -> b.obfuscated = active;
        }
        return b.build();
    }

    public ChatComponent bold(boolean bold) {
        return decoration(ChatDecoration.BOLD, bold);
    }

    public ChatComponent italic(boolean italic) {
        return decoration(ChatDecoration.ITALIC, italic);
    }

    public ChatComponent underlined(boolean underlined) {
        return decoration(ChatDecoration.UNDERLINED, underlined);
    }

    public ChatComponent strikethrough(boolean strikethrough) {
        return decoration(ChatDecoration.STRIKETHROUGH, strikethrough);
    }

    public ChatComponent obfuscated(boolean obfuscated) {
        return decoration(ChatDecoration.OBFUSCATED, obfuscated);
    }

    public ChatComponent insertion(String insertion) {
        BuilderImpl b = copyToBuilder();
        b.insertion = insertion;
        return b.build();
    }

    // ── Composition ────────────────────────────────────────────────────────

    public ChatComponent append(ChatComponent other) {
        BuilderImpl b = copyToBuilder();
        b.children.add(other);
        return b.build();
    }

    public ChatComponent append(String text) {
        return append(ChatComponent.text(text));
    }

    public ChatComponent append(String text, ChatColor color) {
        return append(ChatComponent.text(text, color));
    }

    public ChatComponent newLine() {
        return append(ChatComponent.newline());
    }

    public ChatComponent space() {
        return append(ChatComponent.spaceComponent());
    }

    // ── Click events ───────────────────────────────────────────────────────

    public ChatComponent clickRunCommand(String command) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.runCommand(command);
        return b.build();
    }

    public ChatComponent clickSuggestCommand(String command) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.suggestCommand(command);
        return b.build();
    }

    public ChatComponent clickOpenUrl(String url) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.openUrl(url);
        return b.build();
    }

    public ChatComponent clickCopyToClipboard(String text) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.copyToClipboard(text);
        return b.build();
    }

    // ── Hover events ───────────────────────────────────────────────────────

    public ChatComponent hoverText(ChatComponent text) {
        BuilderImpl b = copyToBuilder();
        b.hoverEvent = ChatHoverEvent.showText(text);
        return b.build();
    }

    public ChatComponent hoverShowItem(Item item) {
        BuilderImpl b = copyToBuilder();
        b.hoverEvent = ChatHoverEvent.showItem(item);
        return b.build();
    }

    public ChatComponent hoverShowEntity(UUID uuid, String entityType, ChatComponent name) {
        BuilderImpl b = copyToBuilder();
        b.hoverEvent = ChatHoverEvent.showEntity(uuid, entityType, name);
        return b.build();
    }

    // ── Accessors ──────────────────────────────────────────────────────────
    @Override
    public ChatComponent[] translatableArgs() {
        return translatableArgs.clone();
    }

    // ── Serialization ──────────────────────────────────────────────────────

    public String toLegacy() {
        StringBuilder sb = new StringBuilder();
        appendLegacy(sb);
        return sb.toString();
    }

    private void appendLegacy(StringBuilder sb) {
        if (color != null && color.hasLegacyCode()) sb.append('§').append(color.legacyCode());
        if (bold) sb.append("§l");
        if (italic) sb.append("§o");
        if (underlined) sb.append("§n");
        if (strikethrough) sb.append("§m");
        if (obfuscated) sb.append("§k");
        sb.append(content);
        for (ChatComponent child : children) child.appendLegacy(sb);
    }

    public String toPlainText() {
        StringBuilder sb = new StringBuilder(content);
        for (ChatComponent child : children) sb.append(child.toPlainText());
        return sb.toString();
    }

    public String toMiniMessage() {
        StringBuilder sb = new StringBuilder();
        appendMiniMessage(sb);
        return sb.toString();
    }

    private void appendMiniMessage(StringBuilder sb) {
        List<String> openTags = new ArrayList<>();
        if (color != null) {
            String tag = color.hasLegacyCode()
                    ? colorNameForMiniMessage(color)
                    : color.asHexString();
            sb.append('<').append(tag).append('>');
            openTags.add(tag);
        }
        if (bold) {
            sb.append("<bold>");
            openTags.add("bold");
        }
        if (italic) {
            sb.append("<italic>");
            openTags.add("italic");
        }
        if (underlined) {
            sb.append("<underlined>");
            openTags.add("underlined");
        }
        if (strikethrough) {
            sb.append("<strikethrough>");
            openTags.add("strikethrough");
        }
        if (obfuscated) {
            sb.append("<obfuscated>");
            openTags.add("obfuscated");
        }
        if (clickEvent != null) {
            String action = switch (clickEvent.action()) {
                case RUN_COMMAND -> "run_command";
                case SUGGEST_COMMAND -> "suggest_command";
                case OPEN_URL -> "open_url";
                case COPY_TO_CLIPBOARD -> "copy_to_clipboard";
            };
            sb.append("<click:").append(action).append(":\"").append(sanitizeMiniMessageValue(clickEvent.value())).append("\">");
            openTags.add("click");
        }
        // Note: hover events are not serialized to MiniMessage format (complex nested structure)
        sb.append(stripLegacyCodes(content));
        for (ChatComponent child : children) child.appendMiniMessage(sb);
        for (int i = openTags.size() - 1; i >= 0; i--) {
            sb.append("</").append(openTags.get(i)).append('>');
        }
    }

    public String toHtml() {
        StringBuilder sb = new StringBuilder();
        appendHtml(sb);
        return sb.toString();
    }

    private static String htmlEscape(String text) {
        return text
                .replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
                .replace("\"", "&quot;")
                .replace("'", "&#x27;");
    }

    private void appendHtml(StringBuilder sb) {
        if (TYPE_NEWLINE.equals(type)) {
            sb.append("<br>");
            for (ChatComponent child : children) child.appendHtml(sb);
            return;
        }

        // Collect open/close tags in outermost-first order
        List<String> openTags = new ArrayList<>();
        List<String> closeTags = new ArrayList<>();

        // 1. Click event (outermost)
        if (clickEvent != null) {
            switch (clickEvent.action()) {
                case RUN_COMMAND -> {
                    openTags.add("<span class=\"helix-click-run\" data-command=\"" + htmlEscape(clickEvent.value()) + "\">");
                    closeTags.addFirst("</span>");
                }
                case SUGGEST_COMMAND -> {
                    openTags.add("<span class=\"helix-click-suggest\" data-command=\"" + htmlEscape(clickEvent.value()) + "\">");
                    closeTags.addFirst("</span>");
                }
                case OPEN_URL -> {
                    openTags.add("<a href=\"" + htmlEscape(clickEvent.value()) + "\" target=\"_blank\" rel=\"noopener noreferrer\">");
                    closeTags.addFirst("</a>");
                }
                case COPY_TO_CLIPBOARD -> {
                    openTags.add("<span class=\"helix-click-copy\" data-value=\"" + htmlEscape(clickEvent.value()) + "\">");
                    closeTags.addFirst("</span>");
                }
            }
        }

        // 2. Hover event
        if (hoverEvent != null) {
            switch (hoverEvent.type()) {
                case SHOW_TEXT -> {
                    String title = htmlEscape(hoverEvent.text().toPlainText());
                    openTags.add("<span title=\"" + title + "\">");
                    closeTags.addFirst("</span>");
                }
                case SHOW_ITEM -> {
                    Item item = hoverEvent.item();
                    if (item != null) {
                        openTags.add("<span class=\"helix-hover-item\" data-item=\"" + htmlEscape(item.materialKey())
                                + "\" title=\"" + htmlEscape(item.displayName()) + "\">");
                        closeTags.addFirst("</span>");
                    }
                }
                case SHOW_ENTITY -> {
                    openTags.add("<span class=\"helix-hover-entity\">");
                    closeTags.addFirst("</span>");
                }
            }
        }

        // 3. Obfuscated
        if (obfuscated) {
            openTags.add("<span class=\"helix-obfuscated\">");
            closeTags.addFirst("</span>");
        }

        // 4. Color
        if (color != null) {
            openTags.add("<span style=\"color: " + color.asHexString() + ";\">");
            closeTags.addFirst("</span>");
        }

        // 5. Bold
        if (bold) {
            openTags.add("<strong>");
            closeTags.addFirst("</strong>");
        }

        // 6. Italic
        if (italic) {
            openTags.add("<em>");
            closeTags.addFirst("</em>");
        }

        // 7. Underlined
        if (underlined) {
            openTags.add("<u>");
            closeTags.addFirst("</u>");
        }

        // 8. Strikethrough
        if (strikethrough) {
            openTags.add("<s>");
            closeTags.addFirst("</s>");
        }

        for (String t : openTags) sb.append(t);
        sb.append(htmlEscape(content));
        for (ChatComponent child : children) child.appendHtml(sb);
        for (String t : closeTags) sb.append(t);
    }

    /**
     * Strips §-prefixed legacy color/format codes from a string so MiniMessage won't reject it.
     */
    private static String stripLegacyCodes(String text) {
        if (text == null || text.indexOf('\u00A7') < 0) return text;
        StringBuilder out = new StringBuilder(text.length());
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            if (c == '\u00A7' && i + 1 < text.length()) {
                i++; // skip the code character after §
            } else {
                out.append(c);
            }
        }
        return out.toString();
    }

    private static String sanitizeMiniMessageValue(String value) {
        // Escape characters that would break MiniMessage tag syntax
        return value.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'");
    }

    private static String colorNameForMiniMessage(ChatColor c) {
        // Compare by rgb value, not reference, to handle cross-classloader ChatColor instances
        return switch (c.rgb()) {
            case 0x000000 -> "black";
            case 0x0000AA -> "dark_blue";
            case 0x00AA00 -> "dark_green";
            case 0x00AAAA -> "dark_aqua";
            case 0xAA0000 -> "dark_red";
            case 0xAA00AA -> "dark_purple";
            case 0xFFAA00 -> "gold";
            case 0xAAAAAA -> "gray";
            case 0x555555 -> "dark_gray";
            case 0x5555FF -> "blue";
            case 0x55FF55 -> "green";
            case 0x55FFFF -> "aqua";
            case 0xFF5555 -> "red";
            case 0xFF55FF -> "light_purple";
            case 0xFFFF55 -> "yellow";
            case 0xFFFFFF -> "white";
            default -> c.asHexString(); // unknown named color — fall back to hex
        };
    }

    // ── Builder ────────────────────────────────────────────────────────────

    public static Builder builder() {
        return new BuilderImpl(TYPE_TEXT, "", null, null, null);
    }

    public interface Builder {
        Builder content(String text);

        Builder color(ChatColor color);

        Builder decoration(ChatDecoration decoration, boolean active);

        Builder bold(boolean bold);

        Builder italic(boolean italic);

        Builder underlined(boolean underlined);

        Builder strikethrough(boolean strikethrough);

        Builder obfuscated(boolean obfuscated);

        Builder insertion(String insertion);

        Builder append(ChatComponent component);

        Builder append(String text);

        Builder newLine();

        Builder clickRunCommand(String command);

        Builder clickSuggestCommand(String command);

        Builder clickOpenUrl(String url);

        Builder clickCopyToClipboard(String text);

        Builder hoverText(ChatComponent text);

        Builder hoverShowItem(Item item);

        ChatComponent build();
    }

    private static final class BuilderImpl implements Builder {
        private final String type;
        private String content;
        private final String translatableKey;
        private final ChatComponent[] translatableArgs;
        private final String keybind;
        private boolean built = false;
        private ChatColor color;
        private boolean bold, italic, underlined, strikethrough, obfuscated;
        private String insertion;
        private ChatClickEvent clickEvent;
        private ChatHoverEvent hoverEvent;
        private final List<ChatComponent> children = new ArrayList<>();

        BuilderImpl(String type, String content, String translatableKey,
                    ChatComponent[] translatableArgs, String keybind) {
            this.type = type;
            this.content = content != null ? content : "";
            this.translatableKey = translatableKey;
            this.translatableArgs = translatableArgs;
            this.keybind = keybind;
        }

        @Override
        public Builder content(String text) {
            if (text == null) throw new NullPointerException("content must not be null");
            this.content = text;
            return this;
        }

        @Override
        public Builder color(ChatColor color) {
            this.color = color;
            return this;
        }

        @Override
        public Builder decoration(ChatDecoration d, boolean active) {
            switch (d) {
                case BOLD -> bold = active;
                case ITALIC -> italic = active;
                case UNDERLINED -> underlined = active;
                case STRIKETHROUGH -> strikethrough = active;
                case OBFUSCATED -> obfuscated = active;
            }
            return this;
        }

        @Override
        public Builder bold(boolean bold) {
            this.bold = bold;
            return this;
        }

        @Override
        public Builder italic(boolean italic) {
            this.italic = italic;
            return this;
        }

        @Override
        public Builder underlined(boolean underlined) {
            this.underlined = underlined;
            return this;
        }

        @Override
        public Builder strikethrough(boolean strikethrough) {
            this.strikethrough = strikethrough;
            return this;
        }

        @Override
        public Builder obfuscated(boolean obfuscated) {
            this.obfuscated = obfuscated;
            return this;
        }

        @Override
        public Builder insertion(String insertion) {
            this.insertion = insertion;
            return this;
        }

        @Override
        public Builder append(ChatComponent c) {
            children.add(c);
            return this;
        }

        @Override
        public Builder append(String text) {
            children.add(ChatComponent.text(text));
            return this;
        }

        @Override
        public Builder newLine() {
            children.add(ChatComponent.newline());
            return this;
        }

        @Override
        public Builder clickRunCommand(String command) {
            this.clickEvent = ChatClickEvent.runCommand(command);
            return this;
        }

        @Override
        public Builder clickSuggestCommand(String command) {
            this.clickEvent = ChatClickEvent.suggestCommand(command);
            return this;
        }

        @Override
        public Builder clickOpenUrl(String url) {
            this.clickEvent = ChatClickEvent.openUrl(url);
            return this;
        }

        @Override
        public Builder clickCopyToClipboard(String text) {
            this.clickEvent = ChatClickEvent.copyToClipboard(text);
            return this;
        }

        @Override
        public Builder hoverText(ChatComponent text) {
            this.hoverEvent = ChatHoverEvent.showText(text);
            return this;
        }

        @Override
        public Builder hoverShowItem(Item item) {
            this.hoverEvent = ChatHoverEvent.showItem(item);
            return this;
        }

        @Override
        public ChatComponent build() {
            if (built) throw new IllegalStateException("Builder has already been used; create a new builder");
            built = true;
            return new ChatComponent(type, content, translatableKey, translatableArgs, keybind,
                    color, bold, italic, underlined, strikethrough, obfuscated,
                    insertion, clickEvent, hoverEvent, children);
        }
    }

    // ── Convenience factories ──────────────────────────────────────────────

    public static ChatComponent playerName(String name) {
        return ChatComponent.text(name)
                .color(ChatColor.AQUA)
                .hoverText(ChatComponent.text("Click to view profile").color(ChatColor.GRAY))
                .clickRunCommand("/whois " + name);
    }

    public static ChatComponent commandSuggestion(String label, String command) {
        return ChatComponent.text(label)
                .color(ChatColor.YELLOW)
                .hoverText(ChatComponent.text("Click to fill: " + command).color(ChatColor.GRAY))
                .clickSuggestCommand(command);
    }

    public static ChatComponent link(String label, String url) {
        return ChatComponent.text(label)
                .color(ChatColor.BLUE)
                .underlined(true)
                .hoverText(ChatComponent.text(url).color(ChatColor.GRAY))
                .clickOpenUrl(url);
    }

    public static ChatComponent copyable(String label, String valueToCopy) {
        return ChatComponent.text(label)
                .color(ChatColor.GREEN)
                .hoverText(ChatComponent.text("Click to copy: " + valueToCopy).color(ChatColor.GRAY))
                .clickCopyToClipboard(valueToCopy);
    }
}
