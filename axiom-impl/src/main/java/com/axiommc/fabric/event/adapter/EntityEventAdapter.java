package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.entity.EntityEvent;
import com.axiommc.api.event.server.BroadcastMessageEvent;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import com.axiommc.fabric.entity.FabricEntity;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.LivingEntity;

/**
 * Provides static hooks for entity spawn and death events fired from mixins,
 * and broadcast message events.
 */
public class EntityEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus bus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        bus = eventBus;
    }

    /**
     * Called from ServerLevelMixin when an entity is added via addFreshEntity.
     * Returns true if the spawn should proceed (not cancelled).
     */
    public static boolean onSpawn(Entity mcEntity, ServerLevel level) {
        if (bus == null) {
            return true;
        }
        FabricEntity entity = new FabricEntity(mcEntity);
        World world = new FabricWorld(level);
        Location location = entity.location();
        EntityEvent.Spawn event = new EntityEvent.Spawn(entity, world, location, "NATURAL");
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from LivingEntityMixin when a living entity dies.
     */
    public static void onDeath(LivingEntity mcEntity) {
        if (bus == null) {
            return;
        }
        FabricEntity entity = new FabricEntity(mcEntity);
        bus.publish(new EntityEvent.Death(entity));
    }

    /**
     * Called from MinecraftServerMixin when a broadcast system message is sent.
     * Returns true if the message should proceed (not cancelled).
     */
    public static boolean onBroadcastMessage(String message) {
        if (bus == null) {
            return true;
        }
        BroadcastMessageEvent event = new BroadcastMessageEvent(message);
        bus.publish(event);
        return !event.isCancelled();
    }
}
