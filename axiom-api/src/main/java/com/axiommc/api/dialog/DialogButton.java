package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;

/**
 * @param tooltip  nullable
 * @param callback nullable = close without callback
 */
public record DialogButton(ChatComponent label, ChatComponent tooltip, DialogCallback callback) {

    // ── Factory methods ───────────────────────────────────────────────────────

    public static DialogButton of(ChatComponent label, DialogCallback callback) {
        return new DialogButton(label, null, callback);
    }

    public static DialogButton of(
        ChatComponent label, ChatComponent tooltip, DialogCallback callback) {
        return new DialogButton(label, tooltip, callback);
    }

    /**
     * Creates a button that closes the dialog without invoking any callback.
     */
    public static DialogButton close(ChatComponent label) {
        return new DialogButton(label, null, null);
    }

    /**
     * Creates a button with a tooltip that closes the dialog without invoking any callback.
     */
    public static DialogButton close(ChatComponent label, ChatComponent tooltip) {
        return new DialogButton(label, tooltip, null);
    }
}
