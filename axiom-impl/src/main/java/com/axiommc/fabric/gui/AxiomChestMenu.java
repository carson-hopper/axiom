package com.axiommc.fabric.gui;

import com.axiommc.api.event.gui.GuiCloseEvent;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.gui.GuiItem;
import com.axiommc.api.gui.GuiSize;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import java.util.UUID;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.AbstractContainerMenu;
import net.minecraft.world.inventory.MenuType;
import net.minecraft.world.item.ItemStack;

public class AxiomChestMenu extends AbstractContainerMenu {

    private final UUID sessionId;
    private Gui gui;
    private final FabricGuiManager manager;
    private final Container container;
    private final ServerPlayer player;

    public AxiomChestMenu(
        int containerId,
        Container container,
        Gui gui,
        UUID sessionId,
        FabricGuiManager manager,
        ServerPlayer player) {
        super(menuType(gui.size()), containerId);
        this.gui = gui;
        this.sessionId = sessionId;
        this.manager = manager;
        this.container = container;
        this.player = player;

        int rows = gui.size().rows();
        int rowOffset = 10;

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < 9; col++) {
                int slotIndex = row * 9 + col;
                GuiItem guiItem = gui.slot(slotIndex);
                addSlot(new GuiSlot(
                    container, slotIndex, 8 + col * 18, rowOffset + row * 18, guiItem, player));
            }
        }
    }

    private static MenuType<?> menuType(GuiSize size) {
        return switch (size) {
            case ONE_ROW -> MenuType.GENERIC_9x1;
            case TWO_ROWS -> MenuType.GENERIC_9x2;
            case THREE_ROWS -> MenuType.GENERIC_9x3;
            case FOUR_ROWS -> MenuType.GENERIC_9x4;
            case FIVE_ROWS -> MenuType.GENERIC_9x5;
            case SIX_ROWS -> MenuType.GENERIC_9x6;
        };
    }

    @Override
    public ItemStack quickMoveStack(Player player, int slot) {
        return ItemStack.EMPTY;
    }

    @Override
    public boolean stillValid(Player player) {
        return true;
    }

    @Override
    public void removed(Player player) {
        super.removed(player);
        FabricPlayer fabricPlayer = new FabricPlayer((ServerPlayer) player);
        gui.handleClose(fabricPlayer);
        Axiom.eventBus().publish(new GuiCloseEvent(fabricPlayer, sessionId));
        manager.removeSession(sessionId);
    }

    public void setGui(Gui newGui) {
        this.gui = newGui;
    }

    public Container getContainer() {
        return container;
    }

    public com.axiommc.api.gui.Gui getGui() {
        return gui;
    }

    private static com.axiommc.api.gui.ClickType toApiClickType(int button) {
        return switch (button) {
            case 0 -> com.axiommc.api.gui.ClickType.LEFT;
            case 1 -> com.axiommc.api.gui.ClickType.RIGHT;
            default -> com.axiommc.api.gui.ClickType.UNKNOWN;
        };
    }
}
