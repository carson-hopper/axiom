package com.axiommc.api.particle;

import java.util.Objects;

public final class ItemData extends ParticleData {
    private final String itemId;

    public ItemData(String itemId) {
        this.itemId = Objects.requireNonNull(itemId, "itemId");
    }

    public String itemId() {
        return itemId;
    }
}
