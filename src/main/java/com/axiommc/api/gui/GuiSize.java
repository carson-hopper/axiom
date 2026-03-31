package com.axiommc.api.gui;

/** Chest inventory size in rows. Each row holds 9 slots. */
public enum GuiSize {
    ONE_ROW(9),
    TWO_ROWS(18),
    THREE_ROWS(27),
    FOUR_ROWS(36),
    FIVE_ROWS(45),
    SIX_ROWS(54);

    private final int slots;

    GuiSize(int slots) {
        this.slots = slots;
    }

    public int slots() { return slots; }
    public int rows()  { return slots / 9; }
}
