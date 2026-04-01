package com.axiommc.api.chat;

import java.util.Objects;

public record ChatClickEvent(Action action, String value) {

    public enum Action {
        RUN_COMMAND,
        SUGGEST_COMMAND,
        OPEN_URL,
        COPY_TO_CLIPBOARD
    }

    public ChatClickEvent(Action action, String value) {
        this.action = action;
        this.value = Objects.requireNonNull(value, "value must not be null");
    }

    public static ChatClickEvent runCommand(String command) {
        return new ChatClickEvent(Action.RUN_COMMAND, command);
    }

    public static ChatClickEvent suggestCommand(String command) {
        return new ChatClickEvent(Action.SUGGEST_COMMAND, command);
    }

    public static ChatClickEvent openUrl(String url) {
        return new ChatClickEvent(Action.OPEN_URL, url);
    }

    public static ChatClickEvent copyToClipboard(String text) {
        return new ChatClickEvent(Action.COPY_TO_CLIPBOARD, text);
    }
}
