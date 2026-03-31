package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;

public record DialogInputOption(String id, ChatComponent display, boolean initial) {

    public static DialogInputOption of(String id, ChatComponent display) {
        return new DialogInputOption(id, display, false);
    }

    public static DialogInputOption of(String id, ChatComponent display, boolean initial) {
        return new DialogInputOption(id, display, initial);
    }

}
