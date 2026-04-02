package com.axiommc.api.item;

/**
 * Represents a Minecraft item type.
 *
 * @param key the namespaced item identifier (e.g. {@code minecraft:diamond_sword})
 */
public record Item(String key) {

    /**
     * Creates an item from a namespaced key.
     *
     * @param key the item key (e.g. {@code minecraft:diamond_sword})
     * @return the item
     */
    public static Item of(String key) {
        return new Item(key);
    }

    @Override
    public String toString() {
        return key;
    }
}
