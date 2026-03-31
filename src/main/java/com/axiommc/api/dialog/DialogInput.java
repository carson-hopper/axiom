package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;

import java.util.List;

public abstract class DialogInput {

    protected final String key;
    protected final ChatComponent label;

    protected DialogInput(String key, ChatComponent label) {
        this.key = key;
        this.label = label;
    }

    public String key() { return key; }
    public ChatComponent label() { return label; }

    // ── Bool ──────────────────────────────────────────────────────────────────

    public static final class Bool extends DialogInput {
        private final boolean initial;

        public Bool(String key, ChatComponent label, boolean initial) {
            super(key, label);
            this.initial = initial;
        }

        public boolean isInitial() { return initial; }
    }

    public static Bool bool(String key, ChatComponent label) {
        return new Bool(key, label, false);
    }

    public static Bool bool(String key, ChatComponent label, boolean initial) {
        return new Bool(key, label, initial);
    }

    // ── Text ──────────────────────────────────────────────────────────────────

    public static final class Text extends DialogInput {
        private final String initial;
        private final int maxLength;

        public Text(String key, ChatComponent label, String initial, int maxLength) {
            super(key, label);
            this.initial = initial;
            this.maxLength = maxLength;
        }

        public String initial() { return initial; }
        public int maxLength() { return maxLength; }
    }

    public static Text text(String key, ChatComponent label) {
        return new Text(key, label, "", 32);
    }

    public static Text text(String key, ChatComponent label, String initial, int maxLength) {
        return new Text(key, label, initial, maxLength);
    }

    // ── NumberRange ───────────────────────────────────────────────────────────

    public static final class NumberRange extends DialogInput {
        private final float min;
        private final float max;
        private final float initial;
        private final float step;

        public NumberRange(String key, ChatComponent label, float min, float max, float initial, float step) {
            super(key, label);
            this.min = min;
            this.max = max;
            this.initial = initial;
            this.step = step;
        }

        public float min() { return min; }
        public float max() { return max; }
        public float initial() { return initial; }
        public float step() { return step; }
    }

    public static NumberRange numberRange(String key, ChatComponent label, float min, float max) {
        return new NumberRange(key, label, min, max, min, 1.0f);
    }

    public static NumberRange numberRange(String key, ChatComponent label, float min, float max, float initial, float step) {
        return new NumberRange(key, label, min, max, initial, step);
    }

    // ── SingleOption ─────────────────────────────────────────────────────────

    public static final class SingleOption extends DialogInput {
        private final List<DialogInputOption> options;

        public SingleOption(String key, ChatComponent label, List<DialogInputOption> options) {
            super(key, label);
            this.options = List.copyOf(options);
        }

        public List<DialogInputOption> options() { return options; }
    }

    public static SingleOption singleOption(String key, ChatComponent label, List<DialogInputOption> options) {
        return new SingleOption(key, label, options);
    }

}
