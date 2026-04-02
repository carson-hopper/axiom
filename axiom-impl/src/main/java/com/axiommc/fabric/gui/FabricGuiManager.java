package com.axiommc.fabric.gui;

import com.axiommc.api.event.gui.GuiCloseEvent;
import com.axiommc.api.event.gui.GuiOpenEvent;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.gui.GuiItem;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.core.Holder;
import net.minecraft.core.component.DataComponents;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.SimpleMenuProvider;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.Items;
import net.minecraft.world.item.component.ItemLore;

import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.UUID;
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

        Component title = serializer.serialize(gui.title());

        sp.openMenu(new SimpleMenuProvider(
            (id, inventory, p) -> new AxiomChestMenu(id, container, gui, sessionId, this, sp),
            title
        ));

        // Get the menu from the player's current container
        AxiomChestMenu menu = (AxiomChestMenu) sp.containerMenu;

        GuiSession session = new GuiSession(sessionId, sp, menu, gui);
        sessions.put(sessionId, session);

        Axiom.eventBus().publish(new GuiOpenEvent(player, sessionId, gui));
        return sessionId;
    }

    @Override
    public void update(UUID sessionId, Gui newGui) {
        GuiSession session = sessions.get(sessionId);
        if (session == null) {
            return;
        }

        Container container = session.menu().getContainer();
        fillContainer(container, newGui);
        session.menu().setGui(newGui);
        session.player().containerMenu.broadcastChanges();
    }

    @Override
    public void close(UUID sessionId) {
        GuiSession session = sessions.get(sessionId);
        if (session == null) {
            return;
        }

        FabricPlayer fabricPlayer = new FabricPlayer(session.player());
        session.player().closeContainer();
        sessions.remove(sessionId);
        Axiom.eventBus().publish(new GuiCloseEvent(fabricPlayer, sessionId));
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
            GuiItem item = gui.slot(i);
            ItemStack itemStack = item != null ? toItemStack(item.item()) : ItemStack.EMPTY;
            container.setItem(i, itemStack);
        }
    }

    private ItemStack toItemStack(com.axiommc.api.chat.Item item) {
        Item mcItem = BuiltInRegistries.ITEM.get(net.minecraft.resources.Identifier.parse(item.materialKey()))
            .map(Holder.Reference::value)
            .orElse(Items.BARRIER);
        ItemStack stack = new ItemStack(mcItem, item.count());

        if (!item.displayName().isEmpty()) {
            Component displayName = Component.literal(item.displayName())
                .withStyle(s -> s.withItalic(false));
            stack.set(DataComponents.CUSTOM_NAME, displayName);
        }

        if (!item.lore().isEmpty()) {
            List<Component> loreComponents = item.lore().stream()
                .map(l -> (Component) Component.literal(l).withStyle(s -> s.withItalic(false)))
                .toList();
            stack.set(DataComponents.LORE, new ItemLore(loreComponents));
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
            Arrays.fill(items, ItemStack.EMPTY);
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
