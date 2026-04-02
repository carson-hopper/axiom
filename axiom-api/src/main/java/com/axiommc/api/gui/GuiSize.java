package com.axiommc.api.gui;

/**
 * Chest GUI size in rows.
 *
 * <p>Each row contains 9 slots. Sizes range from one row (9 slots) to
 * six rows (54 slots).
 */
public enum GuiSize {
    /**
     * Single row GUI (9 slots).
     */
    ONE_ROW(9),

    /**
     * Two row GUI (18 slots).
     */
    TWO_ROWS(18),

    /**
     * Three row GUI (27 slots).
     */
    THREE_ROWS(27),

    /**
     * Four row GUI (36 slots).
     */
    FOUR_ROWS(36),

    /**
     * Five row GUI (45 slots).
     */
    FIVE_ROWS(45),

    /**
     * Six row GUI (54 slots).
     */
    SIX_ROWS(54);

    private final int slots;

    GuiSize(int slots) {
        this.slots = slots;
    }

    /**
     * Gets the total number of slots in this GUI size.
     *
     * @return the number of slots
     */
    public int slots() {
        return slots;
    }

    /**
     * Gets the number of rows in this GUI size.
     *
     * @return the number of rows
     */
    public int rows() {
        return slots / 9;
    }
}
