package com.axiommc.api.chat;

import java.util.Collections;
import java.util.List;
import java.util.Objects;

/**
 * Represents an item for display in chat hover events.
 *
 * <p>Items are typically used in chat components to show what item appears
 * when the player hovers over the text. This record is immutable.
 *
 * @param materialKey the Minecraft material identifier (e.g., "minecraft:diamond_sword")
 * @param count the number of items in the stack
 * @param displayName the custom display name of the item
 * @param lore the item lore (tooltip lines)
 */
public record Item(String materialKey, int count, String displayName, List<String> lore) {

    public Item(String materialKey, int count, String displayName, List<String> lore) {
        this.materialKey = materialKey;
        this.count = count;
        this.displayName = displayName;
        this.lore = Collections.unmodifiableList(lore);
    }

    /**
     * Creates an item with only a material key.
     *
     * <p>Count defaults to 1, display name is empty, and lore is empty.
     *
     * @param materialKey the material identifier (must not be null)
     * @return a new item
     */
    public static Item of(String materialKey) {
        return new Item(
            Objects.requireNonNull(materialKey, "materialKey must not be null"),
            1,
            "",
            Collections.emptyList());
    }

    /**
     * Creates an item with a material key and display name.
     *
     * <p>Count defaults to 1 and lore is empty.
     *
     * @param materialKey the material identifier (must not be null)
     * @param displayName the custom display name (must not be null)
     * @return a new item
     */
    public static Item of(String materialKey, String displayName) {
        return new Item(
            Objects.requireNonNull(materialKey, "materialKey must not be null"),
            1,
            Objects.requireNonNull(displayName, "displayName must not be null"),
            Collections.emptyList());
    }

    /**
     * Creates an item with all attributes.
     *
     * @param materialKey the material identifier (must not be null)
     * @param count the stack size
     * @param displayName the custom display name (must not be null)
     * @param lore the item lore lines (must not be null)
     * @return a new item
     */
    public static Item of(String materialKey, int count, String displayName, List<String> lore) {
        return new Item(
            Objects.requireNonNull(materialKey, "materialKey must not be null"),
            count,
            Objects.requireNonNull(displayName, "displayName must not be null"),
            Objects.requireNonNull(lore, "lore must not be null"));
    }
}
