package com.axiommc.api.chat;

import java.util.UUID;

/**
 * Defines a hover event for chat components. When a player hovers over a component with an
 * attached hover event, the specified content is displayed as a tooltip.
 */
public record ChatHoverEvent(Type type, ChatComponent text, Item item, UUID entityUuid, String entityType,
                             ChatComponent entityName) {

    /**
     * The type of hover event: showing text, an item tooltip, or an entity information panel.
     */
    public enum Type {SHOW_TEXT, SHOW_ITEM, SHOW_ENTITY}

    /**
     * Creates a hover event that displays the given chat component as a tooltip.
     *
     * @param text the tooltip text (must not be null)
     * @return a new hover event of type SHOW_TEXT
     * @throws NullPointerException if text is null
     */
    public static ChatHoverEvent showText(ChatComponent text) {
        if (text == null) {
            throw new NullPointerException("text must not be null");
        }
        return new ChatHoverEvent(Type.SHOW_TEXT, text, null, null, null, null);
    }

    /**
     * Creates a hover event that displays the given item's information as a tooltip.
     *
     * @param item the item to display (must not be null)
     * @return a new hover event of type SHOW_ITEM
     * @throws NullPointerException if item is null
     */
    public static ChatHoverEvent showItem(Item item) {
        if (item == null) {
            throw new NullPointerException("item must not be null");
        }
        return new ChatHoverEvent(Type.SHOW_ITEM, null, item, null, null, null);
    }

    /**
     * Creates a hover event that displays entity information (name, type, UUID).
     *
     * @param uuid the UUID of the entity
     * @param type the entity type namespace (e.g., "minecraft:zombie")
     * @param name the display name of the entity
     * @return a new hover event of type SHOW_ENTITY
     */
    public static ChatHoverEvent showEntity(UUID uuid, String type, ChatComponent name) {
        return new ChatHoverEvent(Type.SHOW_ENTITY, null, null, uuid, type, name);
    }
}
