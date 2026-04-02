package com.axiommc.api.chat;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.UUID;

/**
 * Immutable chat component with styling and interactivity.
 *
 * <p>Represents a formatted chat message or part thereof. Supports colors,
 * decorations (bold, italic, etc.), click events, hover events, and child
 * components for composition. Use factory methods like {@link #text(String)}
 * and {@link #builder()} to create components.
 *
 * @param type the component type (text, translatable, keybind, etc.)
 * @param content the text content for text/keybind components
 * @param translatableKey the translation key for translatable components
 * @param translatableArgs replacement arguments for translatable components
 * @param keybind the keybind key for keybind components
 * @param color the text color
 * @param bold whether to apply bold formatting
 * @param italic whether to apply italic formatting
 * @param underlined whether to apply underline formatting
 * @param strikethrough whether to apply strikethrough formatting
 * @param obfuscated whether to apply obfuscation
 * @param insertion text inserted into chat on shift-click
 * @param clickEvent click event or null
 * @param hoverEvent hover event or null
 * @param children child components
 */
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

    /**
     * Creates a plain-text chat component.
     *
     * @param content the text content (must not be null)
     * @return a new text component
     * @throws NullPointerException if content is null
     */
    public static ChatComponent text(String content) {
        if (content == null) {
            throw new NullPointerException("content must not be null");
        }
        return new BuilderImpl(TYPE_TEXT, content, null, null, null).build();
    }

    /**
     * Creates a plain-text component using String.format.
     *
     * @param format the format string
     * @param args the format arguments
     * @return a new text component with the formatted string
     */
    public static ChatComponent textf(String format, Object... args) {
        return text(format(format, args));
    }

    /**
     * Creates a colored plain-text component.
     *
     * @param content the text content
     * @param color the text color
     * @return a new text component with the specified color
     */
    public static ChatComponent text(String content, ChatColor color) {
        return text(content).color(color);
    }

    /**
     * Creates a styled plain-text component.
     *
     * @param content the text content
     * @param color the text color
     * @param decorations formatting decorations to apply
     * @return a new text component with the specified color and decorations
     */
    public static ChatComponent text(String content, ChatColor color, ChatDecoration... decorations) {
        ChatComponent c = text(content).color(color);
        for (ChatDecoration d : decorations) {
            c = c.decoration(d, true);
        }
        return c;
    }

    /**
     * Creates a translatable component that displays a localized string.
     *
     * @param key the translation key
     * @param args optional replacement arguments for the translated string
     * @return a new translatable component
     */
    public static ChatComponent translatable(String key, ChatComponent... args) {
        return new BuilderImpl(TYPE_TRANSLATABLE, "", key, args, null).build();
    }

    /**
     * Creates a component that displays the name of a keybind.
     *
     * @param keybind the keybind key (e.g., "key.forward", "key.sprint")
     * @return a new keybind component
     */
    public static ChatComponent keybind(String keybind) {
        // Use keybind value as content so toPlainText() returns the keybind string
        return new BuilderImpl(TYPE_KEYBIND, keybind, null, null, keybind).build();
    }

    /** Creates an empty component with no text. */
    public static ChatComponent empty() {
        return text("");
    }

    /** Creates a newline component. */
    public static ChatComponent newline() {
        return new BuilderImpl(TYPE_NEWLINE, "\n", null, null, null).build();
    }

    private static ChatComponent spaceComponent() {
        return new BuilderImpl(TYPE_SPACE, " ", null, null, null).build();
    }

    // ── Styling (immutable) ────────────────────────────────────────────────

    /**
     * Applies a color to this component.
     *
     * @param color the color to apply
     * @return a new component with the specified color
     */
    public ChatComponent color(ChatColor color) {
        BuilderImpl b = copyToBuilder();
        b.color = color;
        return b.build();
    }

    /**
     * Applies or removes a decoration to this component.
     *
     * @param decoration the decoration to apply
     * @param active true to apply, false to remove
     * @return a new component with the updated decoration
     */
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

    /**
     * Applies or removes bold formatting to this component.
     *
     * @param bold true to apply, false to remove
     * @return a new component with the updated formatting
     */
    public ChatComponent bold(boolean bold) {
        return decoration(ChatDecoration.BOLD, bold);
    }

    /**
     * Applies or removes italic formatting to this component.
     *
     * @param italic true to apply, false to remove
     * @return a new component with the updated formatting
     */
    public ChatComponent italic(boolean italic) {
        return decoration(ChatDecoration.ITALIC, italic);
    }

    /**
     * Applies or removes underline formatting to this component.
     *
     * @param underlined true to apply, false to remove
     * @return a new component with the updated formatting
     */
    public ChatComponent underlined(boolean underlined) {
        return decoration(ChatDecoration.UNDERLINED, underlined);
    }

    /**
     * Applies or removes strikethrough formatting to this component.
     *
     * @param strikethrough true to apply, false to remove
     * @return a new component with the updated formatting
     */
    public ChatComponent strikethrough(boolean strikethrough) {
        return decoration(ChatDecoration.STRIKETHROUGH, strikethrough);
    }

    /**
     * Applies or removes obfuscation to this component.
     *
     * @param obfuscated true to apply, false to remove
     * @return a new component with the updated formatting
     */
    public ChatComponent obfuscated(boolean obfuscated) {
        return decoration(ChatDecoration.OBFUSCATED, obfuscated);
    }

    /**
     * Sets the shift-click insertion text for this component.
     *
     * <p>When a player shift-clicks this component, the insertion text is
     * placed in their chat input.
     *
     * @param insertion the text to insert (null to remove)
     * @return a new component with the specified insertion text
     */
    public ChatComponent insertion(String insertion) {
        BuilderImpl b = copyToBuilder();
        b.insertion = insertion;
        return b.build();
    }

    // ── Composition ────────────────────────────────────────────────────────

    /**
     * Appends a child component to this component.
     *
     * @param other the component to append
     * @return a new component with the appended child
     */
    public ChatComponent append(ChatComponent other) {
        BuilderImpl b = copyToBuilder();
        b.children.add(other);
        return b.build();
    }

    /**
     * Appends a plain text child component to this component.
     *
     * @param text the text to append
     * @return a new component with the appended child
     */
    public ChatComponent append(String text) {
        return append(ChatComponent.text(text));
    }

    /**
     * Appends a colored text child component to this component.
     *
     * @param text the text to append
     * @param color the color of the text
     * @return a new component with the appended child
     */
    public ChatComponent append(String text, ChatColor color) {
        return append(ChatComponent.text(text, color));
    }

    /**
     * Appends a newline to this component.
     *
     * @return a new component with a newline appended
     */
    public ChatComponent newLine() {
        return append(ChatComponent.newline());
    }

    /**
     * Appends a space to this component.
     *
     * @return a new component with a space appended
     */
    public ChatComponent space() {
        return append(ChatComponent.spaceComponent());
    }

    // ── Click events ───────────────────────────────────────────────────────

    /**
     * Sets a click event to run a command.
     *
     * <p>When clicked, the specified command is executed on the server.
     *
     * @param command the command to execute
     * @return a new component with the click event
     */
    public ChatComponent clickRunCommand(String command) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.runCommand(command);
        return b.build();
    }

    /**
     * Sets a click event to suggest a command.
     *
     * <p>When clicked, the specified command is placed in the player's chat
     * input without being executed.
     *
     * @param command the command to suggest
     * @return a new component with the click event
     */
    public ChatComponent clickSuggestCommand(String command) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.suggestCommand(command);
        return b.build();
    }

    /**
     * Sets a click event to open a URL.
     *
     * <p>When clicked, the specified URL is opened in the player's browser.
     *
     * @param url the URL to open
     * @return a new component with the click event
     */
    public ChatComponent clickOpenUrl(String url) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.openUrl(url);
        return b.build();
    }

    /**
     * Sets a click event to copy text to the clipboard.
     *
     * <p>When clicked, the specified text is copied to the player's clipboard.
     *
     * @param text the text to copy
     * @return a new component with the click event
     */
    public ChatComponent clickCopyToClipboard(String text) {
        BuilderImpl b = copyToBuilder();
        b.clickEvent = ChatClickEvent.copyToClipboard(text);
        return b.build();
    }

    // ── Hover events ───────────────────────────────────────────────────────

    /**
     * Sets a hover event to show tooltip text.
     *
     * <p>When the player hovers over this component, the specified text is
     * displayed as a tooltip.
     *
     * @param text the tooltip text
     * @return a new component with the hover event
     */
    public ChatComponent hoverText(ChatComponent text) {
        BuilderImpl b = copyToBuilder();
        b.hoverEvent = ChatHoverEvent.showText(text);
        return b.build();
    }

    /**
     * Sets a hover event to show item details.
     *
     * <p>When the player hovers over this component, the item's details are
     * displayed as if hovering over the item in their inventory.
     *
     * @param item the item to display
     * @return a new component with the hover event
     */
    public ChatComponent hoverShowItem(Item item) {
        BuilderImpl b = copyToBuilder();
        b.hoverEvent = ChatHoverEvent.showItem(item);
        return b.build();
    }

    /**
     * Sets a hover event to show entity details.
     *
     * <p>When the player hovers over this component, the entity's details are
     * displayed.
     *
     * @param uuid the entity's UUID
     * @param entityType the entity type
     * @param name the entity's display name
     * @return a new component with the hover event
     */
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

    /**
     * Converts this component to legacy Minecraft color code format.
     *
     * <p>Uses § prefix for color and decoration codes, compatible with older
     * Minecraft versions.
     *
     * @return a string with legacy color codes
     */
    public String toLegacy() {
        StringBuilder sb = new StringBuilder();
        appendLegacy(sb);
        return sb.toString();
    }

    private void appendLegacy(StringBuilder sb) {
        if (color != null && color.hasLegacyCode()) {
            sb.append('§').append(color.legacyCode());
        }
        if (bold) {
            sb.append("§l");
        }
        if (italic) {
            sb.append("§o");
        }
        if (underlined) {
            sb.append("§n");
        }
        if (strikethrough) {
            sb.append("§m");
        }
        if (obfuscated) {
            sb.append("§k");
        }
        sb.append(content);
        for (ChatComponent child : children) {
            child.appendLegacy(sb);
        }
    }

    /**
     * Converts this component to plain text, excluding decorations.
     *
     * @return the plain text content
     */
    public String toPlainText() {
        StringBuilder sb = new StringBuilder(content);
        for (ChatComponent child : children) {
            sb.append(child.toPlainText());
        }
        return sb.toString();
    }

    /**
     * Converts this component to MiniMessage format.
     *
     * @return a string in MiniMessage format
     */
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
        for (ChatComponent child : children) {
            child.appendMiniMessage(sb);
        }
        for (int i = openTags.size() - 1; i >= 0; i--) {
            sb.append("</").append(openTags.get(i)).append('>');
        }
    }

    /**
     * Converts this component to HTML format.
     *
     * <p>Useful for displaying in web interfaces or documentation.
     *
     * @return a string in HTML format
     */
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
            for (ChatComponent child : children) {
                child.appendHtml(sb);
            }
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

        for (String t : openTags) {
            sb.append(t);
        }
        sb.append(htmlEscape(content));
        for (ChatComponent child : children) {
            child.appendHtml(sb);
        }
        for (String t : closeTags) {
            sb.append(t);
        }
    }

    /**
     * Strips §-prefixed legacy color/format codes from a string so MiniMessage won't reject it.
     */
    private static String stripLegacyCodes(String text) {
        if (text == null || text.indexOf('\u00A7') < 0) {
            return text;
        }
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

    /**
     * Creates a new builder for constructing chat components.
     *
     * @return a new chat component builder
     */
    public static Builder builder() {
        return new BuilderImpl(TYPE_TEXT, "", null, null, null);
    }

    /**
     * Builder for constructing chat components.
     *
     * <p>Use this builder to create complex chat components with multiple
     * styling options, children, and events.
     */
    public interface Builder {
        /**
         * Sets the text content.
         *
         * @param text the text content
         * @return this builder
         */
        Builder content(String text);

        /**
         * Sets the text color.
         *
         * @param color the color
         * @return this builder
         */
        Builder color(ChatColor color);

        /**
         * Sets a decoration state.
         *
         * @param decoration the decoration type
         * @param active true to apply, false to remove
         * @return this builder
         */
        Builder decoration(ChatDecoration decoration, boolean active);

        /**
         * Sets bold state.
         *
         * @param bold true to apply
         * @return this builder
         */
        Builder bold(boolean bold);

        /**
         * Sets italic state.
         *
         * @param italic true to apply
         * @return this builder
         */
        Builder italic(boolean italic);

        /**
         * Sets underline state.
         *
         * @param underlined true to apply
         * @return this builder
         */
        Builder underlined(boolean underlined);

        /**
         * Sets strikethrough state.
         *
         * @param strikethrough true to apply
         * @return this builder
         */
        Builder strikethrough(boolean strikethrough);

        /**
         * Sets obfuscation state.
         *
         * @param obfuscated true to apply
         * @return this builder
         */
        Builder obfuscated(boolean obfuscated);

        /**
         * Sets shift-click insertion text.
         *
         * @param insertion the text to insert
         * @return this builder
         */
        Builder insertion(String insertion);

        /**
         * Appends a child component.
         *
         * @param component the component to append
         * @return this builder
         */
        Builder append(ChatComponent component);

        /**
         * Appends text as a child component.
         *
         * @param text the text to append
         * @return this builder
         */
        Builder append(String text);

        /**
         * Appends a newline.
         *
         * @return this builder
         */
        Builder newLine();

        /**
         * Sets a click event to run a command.
         *
         * @param command the command to run
         * @return this builder
         */
        Builder clickRunCommand(String command);

        /**
         * Sets a click event to suggest a command.
         *
         * @param command the command to suggest
         * @return this builder
         */
        Builder clickSuggestCommand(String command);

        /**
         * Sets a click event to open a URL.
         *
         * @param url the URL to open
         * @return this builder
         */
        Builder clickOpenUrl(String url);

        /**
         * Sets a click event to copy text.
         *
         * @param text the text to copy
         * @return this builder
         */
        Builder clickCopyToClipboard(String text);

        /**
         * Sets a hover event to show text.
         *
         * @param text the text to display
         * @return this builder
         */
        Builder hoverText(ChatComponent text);

        /**
         * Sets a hover event to show an item.
         *
         * @param item the item to display
         * @return this builder
         */
        Builder hoverShowItem(Item item);

        /**
         * Builds the chat component.
         *
         * @return a new chat component
         */
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
            if (text == null) {
                throw new NullPointerException("content must not be null");
            }
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
            if (built) {
                throw new IllegalStateException("Builder has already been used; create a new builder");
            }
            built = true;
            return new ChatComponent(type, content, translatableKey, translatableArgs, keybind,
                    color, bold, italic, underlined, strikethrough, obfuscated,
                    insertion, clickEvent, hoverEvent, children);
        }
    }

    // ── Convenience factories ──────────────────────────────────────────────

    /**
     * Creates a clickable player name component.
     *
     * <p>Displays the player name in aqua color. Clicking runs the
     * {@code /whois} command.
     *
     * @param name the player name
     * @return a formatted player name component
     */
    public static ChatComponent playerName(String name) {
        return ChatComponent.text(name)
                .color(ChatColor.AQUA)
                .hoverText(ChatComponent.text("Click to view profile").color(ChatColor.GRAY))
                .clickRunCommand("/whois " + name);
    }

    /**
     * Creates a clickable command suggestion component.
     *
     * <p>Displays the label in yellow. Clicking suggests the command in
     * the player's chat input.
     *
     * @param label the display label
     * @param command the command to suggest
     * @return a formatted command suggestion component
     */
    public static ChatComponent commandSuggestion(String label, String command) {
        return ChatComponent.text(label)
                .color(ChatColor.YELLOW)
                .hoverText(ChatComponent.text("Click to fill: " + command).color(ChatColor.GRAY))
                .clickSuggestCommand(command);
    }

    /**
     * Creates a clickable hyperlink component.
     *
     * <p>Displays the label in blue with underline. Clicking opens the
     * URL in the player's browser.
     *
     * @param label the display label
     * @param url the URL to open
     * @return a formatted link component
     */
    public static ChatComponent link(String label, String url) {
        return ChatComponent.text(label)
                .color(ChatColor.BLUE)
                .underlined(true)
                .hoverText(ChatComponent.text(url).color(ChatColor.GRAY))
                .clickOpenUrl(url);
    }

    /**
     * Creates a clickable copyable text component.
     *
     * <p>Displays the label in green. Clicking copies the value to the
     * player's clipboard.
     *
     * @param label the display label
     * @param valueToCopy the value to copy
     * @return a formatted copyable component
     */
    public static ChatComponent copyable(String label, String valueToCopy) {
        return ChatComponent.text(label)
                .color(ChatColor.GREEN)
                .hoverText(ChatComponent.text("Click to copy: " + valueToCopy).color(ChatColor.GRAY))
                .clickCopyToClipboard(valueToCopy);
    }

    private static String format(String message, Object... args) {
        if (args == null || args.length == 0) {
            return message;
        }
        StringBuilder sb = new StringBuilder();
        int argIndex = 0;
        int i = 0;
        while (i < message.length()) {
            if (i + 1 < message.length()
                    && message.charAt(i) == '{'
                    && message.charAt(i + 1) == '}') {
                if (argIndex < args.length) {
                    sb.append(args[argIndex++]);
                } else {
                    sb.append("{}");
                }
                i += 2;
            } else {
                sb.append(message.charAt(i));
                i++;
            }
        }
        return sb.toString();
    }
}
