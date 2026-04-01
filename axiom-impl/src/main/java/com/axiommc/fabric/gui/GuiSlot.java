package com.axiommc.fabric.gui;

import com.axiommc.api.gui.GuiClickEvent;
import com.axiommc.api.gui.GuiItem;
import com.axiommc.api.gui.ClickType;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.inventory.Slot;
import net.minecraft.world.item.ItemStack;
import net.minecraft.server.level.ServerPlayer;

public class GuiSlot extends Slot {

    private final GuiItem guiItem;
    private final ServerPlayer player;
    private final int slotIndex;

    public GuiSlot(Container container, int slotIndex, int x, int y, GuiItem guiItem, ServerPlayer player) {
        super(container, slotIndex, x, y);
        this.guiItem = guiItem;
        this.player = player;
        this.slotIndex = slotIndex;
    }

    @Override
    public void setChanged() {
        // Don't propagate changes back to container
    }

    @Override
    public boolean mayPlace(ItemStack itemStack) {
        return false;
    }

    @Override
    public boolean mayPickup(net.minecraft.world.entity.player.Player player) {
        if (guiItem != null && guiItem.hasClickHandler()) {
            FabricPlayer fabricPlayer = new FabricPlayer((ServerPlayer) player);
            guiItem.handleClick(new GuiClickEvent(fabricPlayer, slotIndex, ClickType.LEFT));
        }
        return false;
    }
}
