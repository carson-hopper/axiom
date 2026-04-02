package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerInventoryEvent;
import com.axiommc.api.item.Item;
import com.axiommc.api.item.ItemStack;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerInventoryEvent} subtypes from
 * {@code ServerGamePacketListenerMixin} inventory packet hooks.
 */
public class PlayerInventoryAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerInventoryAdapter.eventBus = eventBus;
        PlayerInventoryAdapter.playerProvider = playerProvider;
    }

    /**
     * @return true if the event was cancelled
     */
    public static boolean onClick(
        ServerPlayer serverPlayer, int slot, net.minecraft.world.item.ItemStack mcStack) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(mcStack);
            PlayerInventoryEvent.Click event =
                new PlayerInventoryEvent.Click(player, slot, itemStack);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInventoryEvent.Click", exception);
            return false;
        }
    }

    public static void onClose(ServerPlayer serverPlayer) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerInventoryEvent.Close(player));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInventoryEvent.Close", exception);
        }
    }

    public static void onOpen(ServerPlayer serverPlayer) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerInventoryEvent.Open(player));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInventoryEvent.Open", exception);
        }
    }

    public static void onHeldItemChange(ServerPlayer serverPlayer, int slot) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerInventoryEvent.HeldItemChange(player, slot));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInventoryEvent.HeldItemChange", exception);
        }
    }

    /**
     * @return true if the event was cancelled
     */
    public static boolean onCreativeSlot(
        ServerPlayer serverPlayer, int slot, net.minecraft.world.item.ItemStack mcStack) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(mcStack);
            PlayerInventoryEvent.CreativeSlot event =
                new PlayerInventoryEvent.CreativeSlot(player, slot, itemStack);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInventoryEvent.CreativeSlot", exception);
            return false;
        }
    }

    private static ItemStack toItemStack(net.minecraft.world.item.ItemStack mcStack) {
        if (mcStack == null || mcStack.isEmpty()) {
            return ItemStack.of(Item.of("minecraft:air"), 0);
        }
        String key =
            mcStack.getItem().builtInRegistryHolder().key().identifier().toString();
        return ItemStack.of(Item.of(key), mcStack.getCount());
    }
}
