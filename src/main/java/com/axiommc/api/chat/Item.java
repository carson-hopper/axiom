package com.axiommc.api.chat;

import java.util.Collections;
import java.util.List;
import java.util.Objects;

public record Item(String materialKey, int count, String displayName, List<String> lore) {

    public Item(String materialKey, int count, String displayName, List<String> lore) {
        this.materialKey = materialKey;
        this.count = count;
        this.displayName = displayName;
        this.lore = Collections.unmodifiableList(lore);
    }

    public static Item of(String materialKey) {
        return new Item(
                Objects.requireNonNull(materialKey, "materialKey must not be null"),
                1, "", Collections.emptyList()
        );
    }

    public static Item of(String materialKey, String displayName) {
        return new Item(
                Objects.requireNonNull(materialKey, "materialKey must not be null"),
                1,
                Objects.requireNonNull(displayName, "displayName must not be null"),
                Collections.emptyList()
        );
    }

    public static Item of(String materialKey, int count, String displayName, List<String> lore) {
        return new Item(
                Objects.requireNonNull(materialKey, "materialKey must not be null"),
                count,
                Objects.requireNonNull(displayName, "displayName must not be null"),
                Objects.requireNonNull(lore, "lore must not be null")
        );
    }
}
