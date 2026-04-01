package com.axiommc.api.chat;

public interface ChatComponentSerializer<T> {
    T serialize(ChatComponent component);
}
