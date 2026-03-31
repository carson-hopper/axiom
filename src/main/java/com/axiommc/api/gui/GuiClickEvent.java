package com.axiommc.api.gui;

import com.axiommc.api.player.Player;

import java.util.Objects;

public record GuiClickEvent(Player player, int slot, ClickType clickType) {

    public GuiClickEvent(Player player, int slot, ClickType clickType) {
        this.player = Objects.requireNonNull(player, "player must not be null");
        this.slot = slot;
        this.clickType = Objects.requireNonNull(clickType, "clickType must not be null");
    }
}
