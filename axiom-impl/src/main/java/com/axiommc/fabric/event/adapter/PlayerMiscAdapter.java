package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerAnimationEvent;
import com.axiommc.api.event.player.PlayerBookEvent;
import com.axiommc.api.event.player.PlayerInputEvent;
import com.axiommc.api.event.player.PlayerToggleEvent;
import com.axiommc.api.item.Item;
import com.axiommc.api.item.ItemStack;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires miscellaneous player events: animations, book edits,
 * flight toggles, and movement input.
 */
public class PlayerMiscAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerMiscAdapter.eventBus = eventBus;
        PlayerMiscAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when a player performs an animation.
     *
     * @param serverPlayer the player performing the animation
     * @param type         the animation type (e.g. "ARM_SWING")
     */
    public static void onAnimation(ServerPlayer serverPlayer, String type) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerAnimationEvent(player, type));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerAnimationEvent", exception);
        }
    }

    /**
     * Called when a player edits a book.
     *
     * @param serverPlayer the player editing the book
     * @param book         the Minecraft item stack of the book
     * @return true if the event was cancelled
     */
    public static boolean onEditBook(
        ServerPlayer serverPlayer, net.minecraft.world.item.ItemStack book) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            ItemStack itemStack = toItemStack(book);
            PlayerBookEvent.Edit event = new PlayerBookEvent.Edit(player, itemStack);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerBookEvent.Edit", exception);
        }
        return false;
    }

    /**
     * Called when a player toggles flight.
     *
     * @param serverPlayer the player toggling flight
     * @param flying       true if the player is now flying
     * @return true if the event was cancelled
     */
    public static boolean onToggleFlight(ServerPlayer serverPlayer, boolean flying) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerToggleEvent.Flight event = new PlayerToggleEvent.Flight(player, flying);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerToggleEvent.Flight", exception);
        }
        return false;
    }

    /**
     * Called when a player sends movement input.
     *
     * @param serverPlayer the player sending input
     * @param forward      true if moving forward
     * @param backward     true if moving backward
     * @param left         true if moving left
     * @param right        true if moving right
     * @param jump         true if jumping
     * @param shift        true if shifting
     * @param sprint       true if sprinting
     */
    public static void onInput(
        ServerPlayer serverPlayer,
        boolean forward,
        boolean backward,
        boolean left,
        boolean right,
        boolean jump,
        boolean shift,
        boolean sprint) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(
                new PlayerInputEvent(player, forward, backward, left, right, jump, shift, sprint));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerInputEvent", exception);
        }
    }

    private static ItemStack toItemStack(net.minecraft.world.item.ItemStack mcStack) {
        String key =
            mcStack.getItem().builtInRegistryHolder().key().identifier().toString();
        return ItemStack.of(Item.of(key), mcStack.getCount());
    }
}
