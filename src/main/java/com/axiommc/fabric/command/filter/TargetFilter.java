package com.axiommc.fabric.command.filter;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.world.entity.Mob;
import net.minecraft.world.entity.PathfinderMob;
import net.minecraft.world.entity.animal.Animal;
import net.minecraft.world.entity.monster.Monster;
import net.minecraft.world.phys.AABB;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.UUID;

public class TargetFilter {

    public static List<LivingEntity> parse(String input, CommandSender sender) {
        if (input.startsWith("filter:")) {
            return parseFilter(input.substring(7), sender);
        }
        return parsePlayer(input, sender);
    }

    private static List<LivingEntity> parsePlayer(String playerName, CommandSender sender) {
        var player = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        List<LivingEntity> result = new ArrayList<>();
        player.ifPresent(result::add);
        return result;
    }

    private static List<LivingEntity> parseFilter(String filter, CommandSender sender) {
        if (filter.startsWith("!")) {
            String negated = filter.substring(1);
            return applyNegation(negated, sender);
        }

        return switch (filter) {
            case "players" -> new ArrayList<>(Axiom.players());
            case "mobs", "entities" -> getAllMobs(sender);
            case "hostile" -> getHostileMobs(sender);
            case "passive", "animals" -> getPassiveMobs(sender);
            case "all" -> getAllTargets(sender);
            default -> parsePlayerFilter(filter, sender);
        };
    }

    private static List<LivingEntity> parsePlayerFilter(String playerName, CommandSender sender) {
        var player = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        List<LivingEntity> result = new ArrayList<>();
        player.ifPresent(result::add);
        return result;
    }

    private static List<LivingEntity> applyNegation(String filter, CommandSender sender) {
        List<LivingEntity> targets = getAllTargets(sender);

        return switch (filter) {
            case "self" -> {
                if (sender.isPlayer()) {
                    Player self = sender.asPlayer().get();
                    UUID selfId = self.id();
                    targets.removeIf(e -> e.id().equals(selfId));
                }
                yield targets;
            }
            case "players" -> {
                var playerIds = Axiom.players().stream()
                        .map(Player::id)
                        .toList();
                targets.removeIf(e -> playerIds.contains(e.id()));
                yield targets;
            }
            default -> applyNegationPlayerFilter(filter, targets);
        };
    }

    private static List<LivingEntity> applyNegationPlayerFilter(String playerName, List<LivingEntity> targets) {
        var targetPlayer = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        if (targetPlayer.isPresent()) {
            UUID playerId = targetPlayer.get().id();
            targets.removeIf(e -> e.id().equals(playerId));
        }

        return targets;
    }

    private static List<LivingEntity> getAllTargets(CommandSender sender) {
        List<LivingEntity> targets = new ArrayList<>(Axiom.players());
        targets.addAll(getAllMobs(sender));
        return targets;
    }

    private static List<LivingEntity> getAllMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : Axiom.worlds()) {
            if (world instanceof FabricWorld fabricWorld) {
                net.minecraft.server.level.ServerLevel level = fabricWorld.level();
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                ((net.minecraft.world.level.EntityGetter) level).getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof net.minecraft.world.entity.LivingEntity && !(entity instanceof net.minecraft.server.level.ServerPlayer)) {
                        mobs.add(new MobWrapper(entity));
                    }
                });
            }
        }

        return mobs;
    }

    private static List<LivingEntity> getHostileMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : Axiom.worlds()) {
            if (world instanceof FabricWorld fabricWorld) {
                net.minecraft.server.level.ServerLevel level = fabricWorld.level();
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                ((net.minecraft.world.level.EntityGetter) level).getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof Monster) {
                        mobs.add(new MobWrapper(entity));
                    }
                });
            }
        }

        return mobs;
    }

    private static List<LivingEntity> getPassiveMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : Axiom.worlds()) {
            if (world instanceof FabricWorld fabricWorld) {
                net.minecraft.server.level.ServerLevel level = fabricWorld.level();
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                ((net.minecraft.world.level.EntityGetter) level).getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof Animal) {
                        mobs.add(new MobWrapper(entity));
                    }
                });
            }
        }

        return mobs;
    }

    private static class MobWrapper implements LivingEntity {
        private final net.minecraft.world.entity.LivingEntity entity;

        MobWrapper(net.minecraft.world.entity.Entity entity) {
            this.entity = (net.minecraft.world.entity.LivingEntity) entity;
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
            entity.setCustomName(net.minecraft.network.chat.Component.literal(name));
        }

        @Override
        public com.axiommc.api.player.Location location() {
            var level = (net.minecraft.server.level.ServerLevel) entity.level();
            var world = new FabricWorld(level);
            var position = new com.axiommc.api.math.Vector3(entity.getX(), entity.getY(), entity.getZ());
            var rotation = new com.axiommc.api.math.Vector2(entity.getYRot(), entity.getXRot());
            return new com.axiommc.api.player.Location(world, position, rotation);
        }

        @Override
        public void teleport(com.axiommc.api.player.Location location) {
            entity.teleportTo(location.position().x(), location.position().y(), location.position().z());
        }

        @Override
        public com.axiommc.api.math.Vector3 velocity() {
            var motion = entity.getDeltaMovement();
            return new com.axiommc.api.math.Vector3(motion.x, motion.y, motion.z);
        }

        @Override
        public void velocity(com.axiommc.api.math.Vector3 velocity) {
            entity.setDeltaMovement(velocity.x(), velocity.y(), velocity.z());
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
        public World world() {
            var level = (net.minecraft.server.level.ServerLevel) entity.level();
            return new FabricWorld(level);
        }

        @Override
        public boolean alive() {
            return entity.isAlive();
        }
    }
}
