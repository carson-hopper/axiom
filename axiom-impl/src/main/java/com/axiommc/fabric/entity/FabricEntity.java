package com.axiommc.fabric.entity;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.phys.Vec3;

import java.util.UUID;

/**
 * Wraps a Minecraft {@link net.minecraft.world.entity.Entity} as an
 * Axiom {@link Entity}. Used for non-living entities like boats,
 * minecarts, and item frames.
 */
public class FabricEntity implements Entity {

    protected final net.minecraft.world.entity.Entity entity;

    public FabricEntity(net.minecraft.world.entity.Entity entity) {
        this.entity = entity;
    }

    /** Returns the underlying Minecraft entity. */
    public net.minecraft.world.entity.Entity handle() {
        return entity;
    }

    @Override
    public UUID id() {
        return entity.getUUID();
    }

    @Override
    public String name() {
        return entity.getType().toShortString();
    }

    @Override
    public String nickname() {
        Component customName = entity.getCustomName();
        return customName != null ? customName.getString() : "";
    }

    @Override
    public void nickname(String name) {
        entity.setCustomName(Component.literal(name));
        entity.setCustomNameVisible(true);
    }

    @Override
    public Location location() {
        World world = world();
        Vector3 position = new Vector3(entity.getX(), entity.getY(), entity.getZ());
        Vector2 rotation = new Vector2(entity.getYRot(), entity.getXRot());
        return new Location(world, position, rotation);
    }

    @Override
    public void teleport(Location location) {
        Vector3 position = location.position();
        entity.teleportTo(position.x(), position.y(), position.z());
        rotation(location.rotation());
    }

    @Override
    public Vector3 velocity() {
        Vec3 delta = entity.getDeltaMovement();
        return new Vector3(delta.x, delta.y, delta.z);
    }

    @Override
    public void velocity(Vector3 velocity) {
        entity.setDeltaMovement(velocity.x(), velocity.y(), velocity.z());
    }

    @Override
    public Vector2 rotation() {
        return new Vector2(entity.getYRot(), entity.getXRot());
    }

    @Override
    public void rotation(Vector2 rotation) {
        entity.setYRot(rotation.yaw());
        entity.setXRot(rotation.pitch());
    }

    @Override
    public World world() {
        if (entity.level() instanceof ServerLevel serverLevel) {
            return new FabricWorld(serverLevel);
        }
        return null;
    }

    @Override
    public boolean alive() {
        return entity.isAlive();
    }
}
