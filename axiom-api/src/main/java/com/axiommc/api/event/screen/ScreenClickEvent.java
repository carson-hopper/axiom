package com.axiommc.api.event.screen;

import com.axiommc.api.event.Event;
import com.axiommc.api.gui.ClickType;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.player.Player;

public class ScreenClickEvent extends Event {

    private final Player player;
    private final Vector2 cursor;
    private final ClickType clickType;

    public ScreenClickEvent(Player player, Vector2 cursor, ClickType clickType) {
        this.player = player;
        this.cursor = cursor;
        this.clickType = clickType;
    }

    public Player player() { return player; }

    public Vector2 getCursor() { return cursor; }

    public ClickType getClickType() { return clickType; }

}
