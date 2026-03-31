package com.axiommc.fabric.gui;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.gui.*;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.entity.player.Inventory;
import net.minecraft.world.inventory.AbstractContainerMenu;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.Items;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class FabricGuiManager implements GuiManager {

    private final Map<UUID, GuiSession> sessions = new ConcurrentHashMap<>();
    private final FabricComponentSerializer serializer = new FabricComponentSerializer();

    @Override
    public UUID open(Player player, Gui gui) {
        ServerPlayer sp = ((FabricPlayer) player).player();

        UUID sessionId = UUID.randomUUID();
        SimpleInventoryContainer container = new SimpleInventoryContainer(gui.size().slots());

        fillContainer(container, gui);

        AxiomChestMenu menu = new AxiomChestMenu(0, container, gui, sessionId, this);
        Component title = serializer.serialize(gui.title());

        // Open the menu with the title
        sp.openMenu(new net.minecraft.world.SimpleMenuProvider(
            (id, inventory, p) -> menu,
            title
        ));

        GuiSession session = new GuiSession(sessionId, sp, menu, gui);
        sessions.put(sessionId, session);

        return sessionId;
    }

    @Override
    public void update(UUID sessionId, Gui newGui) {
        GuiSession session = sessions.get(sessionId);
        if (session == null) return;

        Container container = session.menu().getContainer();
        fillContainer(container, newGui);
        session.menu().setGui(newGui);
        session.player().containerMenu.broadcastChanges();
    }

    @Override
    public void close(UUID sessionId) {
        GuiSession session = sessions.get(sessionId);
        if (session == null) return;

        session.player().closeContainer();
        sessions.remove(sessionId);
    }

    @Override
    public boolean isOpen(UUID sessionId) {
        return sessions.containsKey(sessionId);
    }

    public void removeSession(UUID sessionId) {
        sessions.remove(sessionId);
    }

    private void fillContainer(Container container, Gui gui) {
        for (int i = 0; i < gui.size().slots(); i++) {
            GuiItem item = gui.getSlot(i);
            container.setItem(i, item != null ? toItemStack(item.item()) : ItemStack.EMPTY);
        }
    }

    private ItemStack toItemStack(com.axiommc.api.chat.Item item) {
        // For now, use a placeholder item until full item registry lookup is available
        Item mcItem = Items.BARRIER;
        ItemStack stack = new ItemStack(mcItem, item.count());

        if (!item.displayName().isEmpty()) {
            Component displayName = Component.literal(item.displayName())
                .withStyle(s -> s.withItalic(false));
            stack.set(net.minecraft.core.component.DataComponents.CUSTOM_NAME, displayName);
        }

        if (!item.lore().isEmpty()) {
            List<Component> loreComponents = item.lore().stream()
                .map(l -> (Component) Component.literal(l).withStyle(s -> s.withItalic(false)))
                .toList();
            stack.set(net.minecraft.core.component.DataComponents.LORE,
                new net.minecraft.world.item.component.ItemLore(loreComponents));
        }

        return stack;
    }

    // Simple Container implementation for GUI
    private static class SimpleInventoryContainer implements Container {
        private final ItemStack[] items;

        SimpleInventoryContainer(int size) {
            this.items = new ItemStack[size];
            for (int i = 0; i < size; i++) {
                this.items[i] = ItemStack.EMPTY;
            }
        }

        @Override
        public int getContainerSize() {
            return items.length;
        }

        @Override
        public ItemStack getItem(int slot) {
            return slot >= 0 && slot < items.length ? items[slot] : ItemStack.EMPTY;
        }

        @Override
        public ItemStack removeItem(int slot, int amount) {
            return ItemStack.EMPTY;
        }

        @Override
        public ItemStack removeItemNoUpdate(int slot) {
            return ItemStack.EMPTY;
        }

        @Override
        public void setItem(int slot, ItemStack stack) {
            if (slot >= 0 && slot < items.length) {
                items[slot] = stack;
            }
        }

        @Override
        public void setChanged() {
        }

        @Override
        public boolean stillValid(net.minecraft.world.entity.player.Player player) {
            return true;
        }

        @Override
        public void clearContent() {
            for (int i = 0; i < items.length; i++) {
                items[i] = ItemStack.EMPTY;
            }
        }

        @Override
        public boolean isEmpty() {
            for (ItemStack stack : items) {
                if (!stack.isEmpty()) {
                    return false;
                }
            }
            return true;
        }
    }

}
