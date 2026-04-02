package com.axiommc.api.item;

/**
 * Represents a stack of items — an item type with a count.
 *
 * @param item  the item type
 * @param count the number of items in this stack
 */
public record ItemStack(Item item, int count) {

    /**
     * Creates a stack with a single item.
     *
     * @param item the item type
     * @return a stack of 1
     */
    public static ItemStack of(Item item) {
        return new ItemStack(item, 1);
    }

    /**
     * Creates a stack with the given count.
     *
     * @param item  the item type
     * @param count the stack size
     * @return the item stack
     */
    public static ItemStack of(Item item, int count) {
        return new ItemStack(item, count);
    }

    /**
     * Creates a stack from a namespaced key and count.
     *
     * @param key   the item key (e.g. {@code minecraft:diamond_sword})
     * @param count the stack size
     * @return the item stack
     */
    public static ItemStack of(String key, int count) {
        return new ItemStack(Item.of(key), count);
    }

    @Override
    public String toString() {
        return count == 1 ? item.key() : item.key() + " x" + count;
    }
}
