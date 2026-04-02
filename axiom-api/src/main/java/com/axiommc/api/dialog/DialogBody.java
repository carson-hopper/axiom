package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;

public abstract class DialogBody {

    private DialogBody() {}

    public static DialogBody plainMessage(ChatComponent text) {
        return new PlainMessage(text);
    }

    public static final class PlainMessage extends DialogBody {
        private final ChatComponent text;

        public PlainMessage(ChatComponent text) {
            this.text = text;
        }

        public ChatComponent text() {
            return text;
        }
    }
}
