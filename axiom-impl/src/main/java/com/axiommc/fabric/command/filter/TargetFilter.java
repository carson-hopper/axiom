package com.axiommc.fabric.command.filter;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.entity.FabricLivingEntity;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.entity.animal.Animal;
import net.minecraft.world.entity.boss.enderdragon.EnderDragon;
import net.minecraft.world.entity.boss.enderdragon.EnderDragonPart;
import net.minecraft.world.entity.boss.wither.WitherBoss;
import net.minecraft.world.entity.monster.Monster;
import net.minecraft.world.entity.monster.warden.Warden;
import net.minecraft.world.phys.AABB;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

public class TargetFilter {

    public static List<LivingEntity> parse(String input, CommandSender sender) {
        return parseFilter(input, sender);
    }


    private static List<LivingEntity> parseFilter(String filter, CommandSender sender) {
        // Check for space-separated filters (e.g., "zombie creeper !skeleton")
        if (filter.contains(" ")) {
            return parseMultipleFilters(filter, sender);
        }

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
            default -> parsePlayerOrMobFilter(filter, sender);
        };
    }

    private static List<LivingEntity> parseMultipleFilters(String filterStr, CommandSender sender) {
        Set<LivingEntity> positive = new LinkedHashSet<>();
        Set<LivingEntity> negative = new LinkedHashSet<>();

        String[] parts = filterStr.split("\\s+");
        for (String part : parts) {
            part = part.trim();
            if (part.isEmpty()) continue;

            if (part.startsWith("!")) {
                // Negative filter
                String negated = part.substring(1).trim();
                negative.addAll(parseFilterSingle(negated, sender));
            } else {
                // Positive filter
                positive.addAll(parseFilterSingle(part, sender));
            }
        }

        // If only negative filters (positive is empty), start with all targets
        if (positive.isEmpty() && !negative.isEmpty()) {
            positive.addAll(getAllTargets(sender));
        }

        // Remove negative targets from positive set
        List<UUID> negativeIds = negative.stream().map(LivingEntity::id).toList();
        positive.removeIf(e -> negativeIds.contains(e.id()));

        return new ArrayList<>(positive);
    }

    private static List<LivingEntity> parseFilterSingle(String filter, CommandSender sender) {
        return switch (filter) {
            case "self" -> {
                List<LivingEntity> result = new ArrayList<>();
                if (sender.isPlayer()) {
                    result.add(sender.asPlayer().get());
                }
                yield result;
            }
            case "players" -> new ArrayList<>(Axiom.players());
            case "mobs", "entities" -> getAllMobs(sender);
            case "hostile" -> getHostileMobs(sender);
            case "passive", "animals" -> getPassiveMobs(sender);
            case "all" -> getAllTargets(sender);
            default -> parsePlayerOrMobFilter(filter, sender);
        };
    }

    private static List<LivingEntity> parsePlayerOrMobFilter(String name, CommandSender sender) {
        Optional<Player> player = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(name))
                .map(p -> (Player) p)
                .findFirst();

        if (player.isPresent()) {
            List<LivingEntity> result = new ArrayList<>();
            result.add(player.get());
            return result;
        }

        return getMobsByType(sender, name);
    }

    private static List<LivingEntity> getMobsByType(CommandSender sender, String typeName) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : getWorldsForSender(sender)) {
            if (world instanceof FabricWorld(ServerLevel level)) {
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                level.getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof net.minecraft.world.entity.LivingEntity && !(entity instanceof net.minecraft.server.level.ServerPlayer)) {
                        if (!isBossMob(entity) && matchesEntityType(entity, typeName)) {
                            mobs.add(new FabricLivingEntity((net.minecraft.world.entity.LivingEntity) entity));
                        }
                    }
                });
            }
        }

        return mobs;
    }

    private static boolean matchesEntityType(net.minecraft.world.entity.Entity entity, String typeName) {
        String simpleName = entity.getClass().getSimpleName().toLowerCase();
        return simpleName.equals(typeName.toLowerCase());
    }

    private static boolean isBossMob(net.minecraft.world.entity.Entity entity) {
        return entity instanceof EnderDragon ||
                entity instanceof WitherBoss ||
                entity instanceof Warden ||
                entity instanceof EnderDragonPart;
    }

    private static Collection<World> getWorldsForSender(CommandSender sender) {
        if (sender.isPlayer()) {
            Optional<Player> player = sender.asPlayer();
            if (player.isPresent()) {
                World playerWorld = player.get().location().world();
                return java.util.Collections.singletonList(playerWorld);
            }
        }
        return Axiom.worlds();
    }

    private static List<LivingEntity> applyNegation(String filter, CommandSender sender) {
        List<LivingEntity> targets = getAllTargets(sender);
        List<LivingEntity> toRemove = parseFilterSingle(filter, sender);
        List<UUID> removeIds = toRemove.stream().map(LivingEntity::id).toList();
        targets.removeIf(e -> removeIds.contains(e.id()));
        return targets;
    }

    private static List<LivingEntity> getAllTargets(CommandSender sender) {
        List<LivingEntity> targets = new ArrayList<>();

        if (sender.isPlayer()) {
            Optional<Player> player = sender.asPlayer();
            if (player.isPresent()) {
                World playerWorld = player.get().location().world();
                Axiom.players().stream()
                        .filter(p -> p.location().world().equals(playerWorld))
                        .forEach(targets::add);
            }
        } else {
            targets.addAll(Axiom.players());
        }

        targets.addAll(getAllMobs(sender));
        return targets;
    }

    private static List<LivingEntity> getAllMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : getWorldsForSender(sender)) {
            if (world instanceof FabricWorld(ServerLevel level)) {
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                level.getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof net.minecraft.world.entity.LivingEntity && !(entity instanceof net.minecraft.server.level.ServerPlayer)) {
                        if (!isBossMob(entity)) {
                            mobs.add(new FabricLivingEntity((net.minecraft.world.entity.LivingEntity) entity));
                        }
                    }
                });
            }
        }

        return mobs;
    }

    private static List<LivingEntity> getHostileMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : getWorldsForSender(sender)) {
            if (world instanceof FabricWorld(ServerLevel level)) {
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                level.getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof Monster && !isBossMob(entity)) {
                        mobs.add(new FabricLivingEntity((net.minecraft.world.entity.LivingEntity) entity));
                    }
                });
            }
        }

        return mobs;
    }

    private static List<LivingEntity> getPassiveMobs(CommandSender sender) {
        List<LivingEntity> mobs = new ArrayList<>();

        for (World world : getWorldsForSender(sender)) {
            if (world instanceof FabricWorld(ServerLevel level)) {
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                level.getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof Animal && !isBossMob(entity)) {
                        mobs.add(new FabricLivingEntity((net.minecraft.world.entity.LivingEntity) entity));
                    }
                });
            }
        }

        return mobs;
    }
}
