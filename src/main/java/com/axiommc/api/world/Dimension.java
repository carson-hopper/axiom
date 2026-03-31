package com.axiommc.api.world;

public record Dimension(
        String id,
        DimensionType type,
        boolean hasSkyLight,
        boolean hasCeiling,
        boolean hasFixedTime,
        int minY,
        int height,
        int logicalHeight,
        float ambientLight,
        double coordinateScale
) {
    public int maxY() {
        return minY + height;
    }

    public boolean isOverworld() {
        return type == DimensionType.OVERWORLD;
    }

    public boolean isNether() {
        return type == DimensionType.NETHER;
    }

    public boolean isEnd() {
        return type == DimensionType.THE_END;
    }

    public boolean isCustom() {
        return type == DimensionType.CUSTOM;
    }

}
