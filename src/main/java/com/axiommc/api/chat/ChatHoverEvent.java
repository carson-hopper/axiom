package com.axiommc.api.chat;

import java.util.UUID;

public record ChatHoverEvent(Type type, ChatComponent text, Item item, UUID entityUuid, String entityType,
                             ChatComponent entityName) {

    public enum Type {SHOW_TEXT, SHOW_ITEM, SHOW_ENTITY}

    public static ChatHoverEvent showText(ChatComponent text) {
        if (text == null) throw new NullPointerException("text must not be null");
        return new ChatHoverEvent(Type.SHOW_TEXT, text, null, null, null, null);
    }

    public static ChatHoverEvent showItem(Item item) {
        if (item == null) throw new NullPointerException("item must not be null");
        return new ChatHoverEvent(Type.SHOW_ITEM, null, item, null, null, null);
    }

    public static ChatHoverEvent showEntity(UUID uuid, String type, ChatComponent name) {
        return new ChatHoverEvent(Type.SHOW_ENTITY, null, null, uuid, type, name);
    }
}
