package com.axiommc.fabric.entity;

import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerLevel;

import java.util.UUID;

public class FabricLivingEntity implements LivingEntity {

    protected final net.minecraft.world.entity.LivingEntity entity;

    public FabricLivingEntity(net.minecraft.world.entity.LivingEntity entity) {
        this.entity = entity;
    }

    @Override
    public double health() {
        return entity.getHealth();
    }

    @Override
    public void health(double health) {
        entity.setHealth((float) health);
    }

    @Override
    public double maxHealth() {
        return entity.getMaxHealth();
    }

    @Override
    public void damage(double amount) {
        entity.hurt(entity.level().damageSources().generic(), (float) amount);
    }

    @Override
    public UUID id() {
        return entity.getUUID();
    }

    @Override
    public String name() {
        return entity.getName().getString();
    }

    @Override
    public String nickname() {
        return entity.getDisplayName().getString();
    }

    @Override
    public void nickname(String name) {
        entity.setCustomName(Component.literal(name));
    }

    @Override
    public Location location() {
        var level = (ServerLevel) entity.level();
        var world = new FabricWorld(level);
        var position = new Vector3(entity.getX(), entity.getY(), entity.getZ());
        var rotation = new Vector2(entity.getYRot(), entity.getXRot());
        return new Location(world, position, rotation);
    }

    @Override
    public void teleport(Location location) {
        entity.teleportTo(location.position().x(), location.position().y(), location.position().z());
    }

    @Override
    public Vector3 velocity() {
        var motion = entity.getDeltaMovement();
        return new Vector3(motion.x, motion.y, motion.z);
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
        var level = (ServerLevel) entity.level();
        return new FabricWorld(level);
    }

    @Override
    public boolean alive() {
        return entity.isAlive();
    }

}