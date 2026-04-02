package com.axiommc.api.entity.display;

/** Per-entity brightness override. Both values must be in the range 0–15. */
public record DisplayBrightness(int block, int sky) {

    public DisplayBrightness {
        if (block < 0 || block > 15) {
            throw new IllegalArgumentException("block must be 0–15");
        }
        if (sky < 0 || sky > 15) {
            throw new IllegalArgumentException("sky must be 0–15");
        }
    }
}
