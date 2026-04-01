package com.axiommc.api.particle;

import java.util.Objects;

public final class BlockData extends ParticleData {
    private final String blockId;

    public BlockData(String blockId) {
        this.blockId = Objects.requireNonNull(blockId, "blockId");
    }

    public String blockId() { return blockId; }
}
