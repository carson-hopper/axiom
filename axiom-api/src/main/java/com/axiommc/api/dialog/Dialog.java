package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

public record Dialog(
    ChatComponent title,
    List<DialogBody> body,
    List<DialogInput> inputs,
    DialogType type,
    boolean canCloseWithEscape) {

    public Dialog(
        ChatComponent title,
        List<DialogBody> body,
        List<DialogInput> inputs,
        DialogType type,
        boolean canCloseWithEscape) {
        this.title = title;
        this.body = List.copyOf(body);
        this.inputs = List.copyOf(inputs);
        this.type = type;
        this.canCloseWithEscape = canCloseWithEscape;
    }

    public static Builder builder(ChatComponent title, DialogType type) {
        return new Builder(title, type);
    }

    // ── Builder ───────────────────────────────────────────────────────────────

    public static final class Builder {

        private final ChatComponent title;
        private final DialogType type;
        private final List<DialogBody> body = new ArrayList<>();
        private final List<DialogInput> inputs = new ArrayList<>();
        private boolean canCloseWithEscape = true;

        private Builder(ChatComponent title, DialogType type) {
            this.title = Objects.requireNonNull(title, "title must not be null");
            this.type = Objects.requireNonNull(type, "type must not be null");
        }

        /**
         * Shorthand for adding a plain-message body.
         */
        public Builder body(ChatComponent text) {
            this.body.add(DialogBody.plainMessage(text));
            return this;
        }

        public Builder body(DialogBody... bodies) {
            this.body.addAll(Arrays.asList(bodies));
            return this;
        }

        public Builder inputs(DialogInput... inputs) {
            this.inputs.addAll(Arrays.asList(inputs));
            return this;
        }

        public Builder canCloseWithEscape(boolean v) {
            this.canCloseWithEscape = v;
            return this;
        }

        public Dialog build() {
            return new Dialog(title, body, inputs, type, canCloseWithEscape);
        }
    }
}
