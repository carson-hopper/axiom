package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerBedEvent;
import com.axiommc.api.event.player.PlayerExperienceEvent;
import com.axiommc.api.event.player.PlayerVelocityEvent;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires player state events (bed, experience, velocity) from {@code ServerPlayerMixin}.
 */
public class PlayerStateAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerStateAdapter.eventBus = eventBus;
        PlayerStateAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when a player enters a bed.
     *
     * @param serverPlayer the player entering the bed
     * @param bedLocation  the bed location
     * @return true if the event was cancelled
     */
    public static boolean onBedEnter(ServerPlayer serverPlayer, Location bedLocation) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerBedEvent.Enter event = new PlayerBedEvent.Enter(player, bedLocation);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerBedEvent.Enter", exception);
        }
        return false;
    }

    /**
     * Called when a player leaves a bed.
     *
     * @param serverPlayer the player leaving the bed
     * @param bedLocation  the bed location
     */
    public static void onBedLeave(ServerPlayer serverPlayer, Location bedLocation) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerBedEvent.Leave(player, bedLocation));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerBedEvent.Leave", exception);
        }
    }

    /**
     * Called when a player's experience points change.
     *
     * @param serverPlayer the player gaining experience
     * @param amount       the experience amount
     * @return true if the event was cancelled
     */
    public static boolean onExperienceChange(ServerPlayer serverPlayer, int amount) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerExperienceEvent.Change event = new PlayerExperienceEvent.Change(player, amount);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerExperienceEvent.Change", exception);
        }
        return false;
    }

    /**
     * Called when a player's experience level changes.
     *
     * @param serverPlayer the player whose level changed
     * @param oldLevel     the previous level
     * @param newLevel     the new level
     */
    public static void onLevelChange(ServerPlayer serverPlayer, int oldLevel, int newLevel) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerExperienceEvent.LevelChange(player, oldLevel, newLevel));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerExperienceEvent.LevelChange", exception);
        }
    }

    /**
     * Called when a player's velocity changes (e.g. knockback).
     *
     * @param serverPlayer the player whose velocity changed
     * @param x            the x velocity component
     * @param y            the y velocity component
     * @param z            the z velocity component
     * @return true if the event was cancelled
     */
    public static boolean onVelocityChange(
        ServerPlayer serverPlayer, double x, double y, double z) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Vector3 velocity = new Vector3(x, y, z);
            PlayerVelocityEvent event = new PlayerVelocityEvent(player, velocity);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerVelocityEvent", exception);
        }
        return false;
    }
}
