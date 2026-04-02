package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerDamageEvent;
import com.axiommc.api.event.player.PlayerDeathEvent;
import com.axiommc.api.event.player.PlayerGameModeChangeEvent;
import com.axiommc.api.player.GameMode;
import com.axiommc.api.event.player.PlayerItemEvent;
import com.axiommc.api.event.player.PlayerRideEvent;
import com.axiommc.api.event.player.PlayerPositionEvent;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.level.portal.TeleportTransition;

/**
 * Fires player action events from {@code ServerPlayerMixin}.
 */
public class PlayerActionAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerActionAdapter.eventBus = eventBus;
        PlayerActionAdapter.playerProvider = playerProvider;
    }

    /**
     * Called when a player dies.
     *
     * @param serverPlayer the player who died
     * @param deathMessage the localized death message
     */
    public static void onDeath(ServerPlayer serverPlayer, String deathMessage) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerDeathEvent.Death(player, deathMessage));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerDeathEvent", exception);
        }
    }

    /**
     * Called when a player takes damage.
     *
     * @param serverPlayer the player taking damage
     * @param damage       the damage amount
     * @param cause        the damage source message id
     * @return true if the event was cancelled
     */
    public static boolean onDamage(ServerPlayer serverPlayer, float damage, String cause) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerDamageEvent event = new PlayerDamageEvent(player, damage, cause);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerDamageEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player drops an item.
     *
     * @param serverPlayer the player dropping the item
     * @param item         the item being dropped
     * @return true if the event was cancelled
     */
    public static boolean onDropItem(ServerPlayer serverPlayer, String item) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerItemEvent.Drop event = new PlayerItemEvent.Drop(player, item);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerDropItemEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player picks up an item.
     *
     * @param serverPlayer the player picking up the item
     * @param item         the item being picked up
     * @param amount       the stack count
     */
    public static void onPickupItem(ServerPlayer serverPlayer, String item, int amount) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerItemEvent.Pickup(player, item, amount));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerPickupItemEvent", exception);
        }
    }

    /**
     * Called when a player's game mode changes.
     *
     * @param serverPlayer the player whose game mode is changing
     * @param gameModeName the new game mode name
     * @return true if the event was cancelled
     */
    public static boolean onGameModeChange(ServerPlayer serverPlayer, String gameModeName) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            GameMode gameMode = GameMode.fromName(gameModeName);
            PlayerGameModeChangeEvent event = new PlayerGameModeChangeEvent(player, gameMode);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerGameModeChangeEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player is teleported.
     *
     * @param serverPlayer the player being teleported
     * @param transition   the teleport transition containing destination info
     * @return true if the event was cancelled
     */
    public static boolean onTeleport(ServerPlayer serverPlayer, TeleportTransition transition) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            Location from = player.location();
            Vector3 destination = new Vector3(
                    transition.position().x,
                    transition.position().y,
                    transition.position().z);
            String worldName = transition.newLevel().dimension().identifier().toString();
            World toWorld = Axiom.world(worldName).orElse(null);
            Location to = new Location(toWorld, destination, new Vector2(0, 0));
            PlayerPositionEvent.Teleport event = new PlayerPositionEvent.Teleport(player, from, to);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerPositionEvent.Teleport", exception);
        }
        return false;
    }

    /**
     * Called when a player starts riding an entity.
     *
     * @param serverPlayer the player mounting
     * @param entityId     the id of the entity being mounted
     * @return true if the event was cancelled
     */
    public static boolean onMount(ServerPlayer serverPlayer, int entityId) {
        if (eventBus == null) {
            return false;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            PlayerRideEvent.Mount event = new PlayerRideEvent.Mount(player, entityId);
            eventBus.publish(event);
            return event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerMountEvent", exception);
        }
        return false;
    }

    /**
     * Called when a player stops riding an entity.
     *
     * @param serverPlayer the player dismounting
     * @param entityId     the id of the entity being dismounted
     */
    public static void onDismount(ServerPlayer serverPlayer, int entityId) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerRideEvent.Dismount(player, entityId));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing PlayerDismountEvent", exception);
        }
    }
}
