package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Greedy;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.command.filter.TargetFilter;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.phys.AABB;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

@CommandMeta(
        name = "kill",
        description = "Kill a player or mob",
        permission = "axiom.kill"
)
public class KillCommand implements Command {

    private static final String[] FILTER_OPTIONS = {
        "players",
        "mobs",
        "entities",
        "hostile",
        "passive",
        "animals",
        "all",
        "!self",
        "!players",
        "!mobs",
        "!entities",
        "!hostile",
        "!passive",
        "!animals",
        "!all"
    };

    @Execute
    public void execute(CommandSender sender) {
        sender.asPlayer().ifPresentOrElse(
                player -> {
                    player.damage(player.health());
                    player.sendMessage(ChatComponent.text("You have been killed").color(ChatColor.RED));
                },
                () -> sender.sendMessage(ChatComponent.text("Only players can be killed").color(ChatColor.RED))
        );
    }

    @Execute
    public void execute(CommandSender sender, @Arg("target") @Greedy @DynamicTabComplete("suggestTargets") String target) {
        // Convert space-separated filters to the format TargetFilter expects
        // "/kill zombie creeper !self" -> "filter:zombie creeper !self"
        String fullTarget = target.startsWith("filter:") ? target : "filter:" + target;
        Set<LivingEntity> allTargets = new LinkedHashSet<>(TargetFilter.parse(fullTarget, sender));

        if (allTargets.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No targets found: " + target).color(ChatColor.RED));
            return;
        }

        for (LivingEntity entity : allTargets) {
            if (entity instanceof Player) {
                entity.damage(entity.health());
            } else {
                entity.health(0);
            }
        }

        if (allTargets.size() == 1) {
            sender.sendMessage(ChatComponent.text("Killed " + allTargets.iterator().next().name()).color(ChatColor.RED));
        } else {
            sender.sendMessage(ChatComponent.text("Killed " + allTargets.size() + " entities").color(ChatColor.RED));
        }
    }

    public List<String> suggestTargets(String partial) {
        List<String> suggestions = new ArrayList<>();

        // Check if user already typed "filter:"
        if (partial.startsWith("filter:")) {
            String afterFilter = partial.substring(7);
            String lowerAfterFilter = afterFilter.toLowerCase();

            // Check for space-separated (already has at least one filter)
            if (afterFilter.contains(" ")) {
                int lastSpaceIdx = afterFilter.lastIndexOf(" ");
                String prefix = partial.substring(0, 7 + lastSpaceIdx + 1);
                String lastPart = afterFilter.substring(lastSpaceIdx + 1).trim();
                String lowerLastPart = lastPart.toLowerCase();

                // Check if "all" is already in the filters
                boolean hasAll = afterFilter.toLowerCase().contains("all");
                addAllFilterSuggestions(suggestions, prefix, lowerLastPart, hasAll);
            } else {
                // First filter after "filter:"
                addAllFilterSuggestions(suggestions, "filter:", lowerAfterFilter, false);
            }
        } else {
            // Suggest starting with "filter:" or show direct filters/negations
            if ("filter:".startsWith(partial.toLowerCase())) {
                suggestions.add("filter:");
            }

            String lowerPartial = partial.toLowerCase();

            // Suggest filters directly without filter: prefix
            for (String filter : FILTER_OPTIONS) {
                if (filter.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(filter);
                }
            }

            Set<String> mobTypes = discoverMobTypes();

            // Suggest player names directly
            Axiom.players().forEach(player -> {
                if (player.name().toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(player.name());
                }
            });

            // Suggest mob types directly
            mobTypes.forEach(mobType -> {
                if (mobType.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(mobType);
                }
            });

            // Suggest negative versions directly
            if (lowerPartial.startsWith("!")) {
                String afterNegation = lowerPartial.substring(1);
                Axiom.players().forEach(player -> {
                    if (player.name().toLowerCase().startsWith(afterNegation)) {
                        suggestions.add("!" + player.name());
                    }
                });

                mobTypes.forEach(mobType -> {
                    if (mobType.toLowerCase().startsWith(afterNegation)) {
                        suggestions.add("!" + mobType);
                    }
                });
            }
        }

        return suggestions;
    }

    private void addAllFilterSuggestions(List<String> suggestions, String prefix, String lowerPartial, boolean hasAll) {
        Set<String> mobTypes = discoverMobTypes();

        if (hasAll) {
            // If "all" is already present, only suggest negative filters
            
            // Negative filter keywords
            String[] negativeFilters = {
                "!self",
                "!players",
                "!mobs",
                "!entities",
                "!hostile",
                "!passive",
                "!animals",
                "!all"
            };
            
            for (String filter : negativeFilters) {
                if (filter.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + filter);
                }
            }

            // Negative player names
            Axiom.players().forEach(player -> {
                String negName = "!" + player.name();
                if (negName.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + negName);
                }
            });

            // Negative mob types
            mobTypes.forEach(mobType -> {
                String negType = "!" + mobType;
                if (negType.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + negType);
                }
            });
        } else {
            // "all" not present - suggest both positive and negative
            
            // Positive filter keywords
            for (String filter : FILTER_OPTIONS) {
                if (filter.startsWith("!")) continue;
                if (filter.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + filter);
                }
            }

            // Positive player names
            Axiom.players().forEach(player -> {
                if (player.name().toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + player.name());
                }
            });

            // Positive mob types
            mobTypes.forEach(mobType -> {
                if (mobType.toLowerCase().startsWith(lowerPartial)) {
                    suggestions.add(prefix + mobType);
                }
            });

            // Negative filters (if user typed !)
            if (lowerPartial.startsWith("!")) {
                String afterNegation = lowerPartial.substring(1);
                for (String filter : FILTER_OPTIONS) {
                    if (!filter.startsWith("!")) {
                        if (filter.toLowerCase().startsWith(afterNegation)) {
                            suggestions.add(prefix + "!" + filter);
                        }
                    }
                }

                // Negative player names
                Axiom.players().forEach(player -> {
                    if (player.name().toLowerCase().startsWith(afterNegation)) {
                        suggestions.add(prefix + "!" + player.name());
                    }
                });

                // Negative mob types
                mobTypes.forEach(mobType -> {
                    if (mobType.toLowerCase().startsWith(afterNegation)) {
                        suggestions.add(prefix + "!" + mobType);
                    }
                });
            }
        }
    }

    private Set<String> discoverMobTypes() {
        Set<String> mobTypes = new HashSet<>();

        for (com.axiommc.api.world.World world : Axiom.worlds()) {
            if (world instanceof com.axiommc.fabric.world.FabricWorld fabricWorld) {
                ServerLevel level = fabricWorld.level();
                AABB worldBounds = new AABB(-3e7, -64, -3e7, 3e7, 320, 3e7);
                ((net.minecraft.world.level.EntityGetter) level).getEntities(null, worldBounds).forEach(entity -> {
                    if (entity instanceof net.minecraft.world.entity.LivingEntity && !(entity instanceof net.minecraft.server.level.ServerPlayer)) {
                        String simpleName = entity.getClass().getSimpleName().toLowerCase();
                        mobTypes.add(simpleName);
                    }
                });
            }
        }

        return mobTypes;
    }
}
