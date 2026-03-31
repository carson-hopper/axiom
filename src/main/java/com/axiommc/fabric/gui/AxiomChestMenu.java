package com.axiommc.fabric.gui;

import com.axiommc.api.gui.*;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.*;
import net.minecraft.world.item.ItemStack;
import java.util.UUID;

public class AxiomChestMenu extends AbstractContainerMenu {

    private final UUID sessionId;
    private Gui gui;
    private final FabricGuiManager manager;
    private final Container container;

    public AxiomChestMenu(int containerId, Container container, Gui gui, UUID sessionId, FabricGuiManager manager) {
        super(menuType(gui.size()), containerId);
        this.gui = gui;
        this.sessionId = sessionId;
        this.manager = manager;
        this.container = container;

        int rows = gui.size().rows();
        int rowOffset = 10;

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < 9; col++) {
                addSlot(new Slot(container, row * 9 + col, 8 + col * 18, rowOffset + row * 18));
            }
        }

        addPlayerInventory(container, rowOffset + rows * 18 + 10);
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

    private void addPlayerInventory(Container container, int yOffset) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 9; col++) {
                addSlot(new Slot(container, 27 + row * 9 + col, 8 + col * 18, yOffset + row * 18));
            }
        }
        for (int col = 0; col < 9; col++) {
            addSlot(new Slot(container, col, 8 + col * 18, yOffset + 58));
        }
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
        FabricPlayer fabricPlayer = new FabricPlayer((net.minecraft.server.level.ServerPlayer) player);
        gui.handleClose(fabricPlayer);
        manager.removeSession(sessionId);
    }

    public void setGui(Gui newGui) {
        this.gui = newGui;
    }

    public Container getContainer() {
        return container;
    }

    public void handleSlotClick(int slot, int button) {
        if (slot < 0 || slot >= gui.size().slots()) {
            return;
        }

        GuiItem guiItem = gui.getSlot(slot);
        if (guiItem != null && guiItem.hasClickHandler()) {
            ClickType apiClickType = toApiClickType(button);
            // Note: Player will be obtained from server context when called
            guiItem.handleClick(new GuiClickEvent(null, slot, apiClickType));
        }
    }

    private static ClickType toApiClickType(int button) {
        return switch (button) {
            case 0 -> ClickType.LEFT;
            case 1 -> ClickType.RIGHT;
            default -> ClickType.UNKNOWN;
        };
    }
}
