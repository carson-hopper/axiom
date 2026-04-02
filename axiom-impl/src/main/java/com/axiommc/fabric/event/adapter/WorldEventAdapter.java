package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.world.WorldChunkEvent;
import com.axiommc.api.event.world.WorldLifecycleEvent;
import com.axiommc.api.event.world.WorldSpawnChangeEvent;
import com.axiommc.api.event.world.WorldTimeSkipEvent;
import com.axiommc.api.event.world.WorldWeatherEvent;
import com.axiommc.api.math.Vector3;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.world.FabricWorld;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerChunkEvents;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerLevelEvents;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerLifecycleEvents;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.level.ChunkPos;

/**
 * Bridges Fabric world events (chunk load/unload, world lifecycle, save) to
 * Axiom API events. Also provides static hooks for mixin-driven events
 * (weather, thunder, lightning, time skip, spawn change).
 */
public class WorldEventAdapter implements FabricEventAdapter {

    private static SimpleEventBus bus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        bus = eventBus;

        ServerChunkEvents.CHUNK_LOAD.register((serverLevel, chunk, isNewChunk) -> {
            ChunkPos pos = chunk.getPos();
            eventBus.publish(
                new WorldChunkEvent.Load(new FabricWorld(serverLevel), pos.x(), pos.z()));
        });

        ServerChunkEvents.CHUNK_UNLOAD.register((serverLevel, chunk) -> {
            ChunkPos pos = chunk.getPos();
            eventBus.publish(
                new WorldChunkEvent.Unload(new FabricWorld(serverLevel), pos.x(), pos.z()));
        });

        ServerLevelEvents.LOAD.register((server, serverLevel) -> {
            eventBus.publish(new WorldLifecycleEvent.Load(new FabricWorld(serverLevel)));
        });

        ServerLevelEvents.UNLOAD.register((server, serverLevel) -> {
            eventBus.publish(new WorldLifecycleEvent.Unload(new FabricWorld(serverLevel)));
        });

        ServerLifecycleEvents.BEFORE_SAVE.register((server, flush, force) -> {
            for (ServerLevel level : server.getAllLevels()) {
                eventBus.publish(new WorldLifecycleEvent.Save(new FabricWorld(level)));
            }
        });
    }

    // ── Static mixin hooks ──────────────────────────────────────────────

    /**
     * Called from a mixin when rain state changes. Returns true if the change should proceed.
     */
    public static boolean onWeatherChange(ServerLevel serverLevel, boolean raining) {
        if (bus == null) {
            return true;
        }
        WorldWeatherEvent.Change event =
            new WorldWeatherEvent.Change(new FabricWorld(serverLevel), raining);
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from a mixin when thunder state changes. Returns true if the change should proceed.
     */
    public static boolean onThunderChange(ServerLevel serverLevel, boolean thundering) {
        if (bus == null) {
            return true;
        }
        WorldWeatherEvent.ThunderChange event =
            new WorldWeatherEvent.ThunderChange(new FabricWorld(serverLevel), thundering);
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from a mixin when lightning strikes. Returns true if the strike should proceed.
     */
    public static boolean onLightningStrike(ServerLevel serverLevel, double x, double y, double z) {
        if (bus == null) {
            return true;
        }
        WorldWeatherEvent.LightningStrike event = new WorldWeatherEvent.LightningStrike(
            new FabricWorld(serverLevel), new Vector3(x, y, z));
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from a mixin when time is skipped. Returns true if the skip should proceed.
     */
    public static boolean onTimeSkip(ServerLevel serverLevel, long amount, String reason) {
        if (bus == null) {
            return true;
        }
        WorldTimeSkipEvent event =
            new WorldTimeSkipEvent(new FabricWorld(serverLevel), amount, reason);
        bus.publish(event);
        return !event.isCancelled();
    }

    /**
     * Called from a mixin when the world spawn point changes.
     */
    public static void onSpawnChange(ServerLevel serverLevel, Vector3 oldSpawn, Vector3 newSpawn) {
        if (bus == null) {
            return;
        }
        bus.publish(new WorldSpawnChangeEvent(new FabricWorld(serverLevel), oldSpawn, newSpawn));
    }
}
