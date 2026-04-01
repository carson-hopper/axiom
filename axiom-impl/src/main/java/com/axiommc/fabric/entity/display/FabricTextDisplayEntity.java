package com.axiommc.fabric.entity.display;

import com.axiommc.api.entity.display.TextDisplayEntity;
import com.axiommc.api.entity.display.TextDisplaySpec;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import net.minecraft.network.chat.Component;
import net.minecraft.world.entity.Display;
import net.minecraft.world.phys.Vec3;

import java.util.UUID;
import java.util.function.Consumer;

public record FabricTextDisplayEntity(Display.TextDisplay entity) implements TextDisplayEntity {

    @Override
    public UUID id() {
        return entity.getUUID();
    }

    @Override
    public String name() {
        return "TextDisplay";
    }

    @Override
    public String nickname() {
        return entity.getCustomName() != null ? entity.getCustomName().getString() : "TextDisplay";
    }

    @Override
    public void nickname(String name) {
        // Display entities don't support nicknames; silently ignore
    }

    @Override
    public com.axiommc.api.world.World world() {
        if (entity.level() instanceof net.minecraft.server.level.ServerLevel serverLevel) {
            return new com.axiommc.fabric.world.FabricWorld(serverLevel);
        }
        return null;
    }

    @Override
    public Location location() {
        return new Location(
            world(),
            new Vector3(entity.getX(), entity.getY(), entity.getZ()),
            new com.axiommc.api.math.Vector2(entity.getYRot(), entity.getXRot())
        );
    }

    @Override
    public com.axiommc.api.math.Vector2 rotation() {
        return new com.axiommc.api.math.Vector2(entity.getYRot(), entity.getXRot());
    }

    @Override
    public void rotation(com.axiommc.api.math.Vector2 rotation) {
        entity.setYRot(rotation.yaw());
        entity.setXRot(rotation.pitch());
    }

    @Override
    public Vector3 velocity() {
        Vec3 motion = entity.getDeltaMovement();
        return new Vector3(motion.x, motion.y, motion.z);
    }

    @Override
    public void velocity(Vector3 velocity) {
        entity.setDeltaMovement(velocity.x(), velocity.y(), velocity.z());
    }

    @Override
    public void teleport(Location location) {
        entity.setPos(location.position().x(), location.position().y(), location.position().z());
        entity.setYRot(location.rotation().yaw());
        entity.setXRot(location.rotation().pitch());
    }

    @Override
    public boolean alive() {
        return !entity.isRemoved();
    }

    @Override
    public void remove() {
        entity.discard();
    }

    @Override
    public boolean isRemoved() {
        return entity.isRemoved();
    }

    @Override
    public void update(Consumer<TextDisplaySpec.Builder> editor, int interpolationTicks) {
        // TODO: Implement updating display properties with interpolation
        // This requires extracting current spec, building new spec, and applying changes
    }
}
