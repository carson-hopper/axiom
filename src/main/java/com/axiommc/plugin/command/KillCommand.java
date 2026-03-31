package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
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
        "!players"
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
    public void execute(CommandSender sender, @Arg("target") @DynamicTabComplete("suggestTargets") String target) {
        String fullTarget = target.startsWith("filter:") ? target : target;
        Set<LivingEntity> allTargets = new LinkedHashSet<>(TargetFilter.parse(fullTarget, sender));

        if (allTargets.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No targets found: " + target).color(ChatColor.RED));
            return;
        }

        for (LivingEntity entity : allTargets) {
            entity.damage(entity.health());
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

            // Check for comma-separated (already has at least one filter)
            if (afterFilter.contains(",")) {
                int lastCommaIdx = afterFilter.lastIndexOf(",");
                String prefix = partial.substring(0, 7 + lastCommaIdx + 1);
                String lastPart = afterFilter.substring(lastCommaIdx + 1).trim();
                String lowerLastPart = lastPart.toLowerCase();

                addFilterSuggestions(suggestions, prefix, lowerLastPart);
            } else {
                // First filter after "filter:"
                addFilterSuggestions(suggestions, "filter:", lowerAfterFilter);
            }
        } else {
            // Suggest starting with "filter:"
            if ("filter:".startsWith(partial.toLowerCase())) {
                suggestions.add("filter:");
            }
        }

        return suggestions;
    }

    private void addFilterSuggestions(List<String> suggestions, String prefix, String lowerPartial) {
        // Filter keywords (without filter: prefix since we're already in that context)
        for (String filter : FILTER_OPTIONS) {
            if (filter.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(prefix + filter);
            }
        }

        Set<String> mobTypes = discoverMobTypes();

        // Player names
        Axiom.players().forEach(player -> {
            if (player.name().toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(prefix + player.name());
            }
        });

        // Mob types
        mobTypes.forEach(mobType -> {
            if (mobType.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(prefix + mobType);
            }
        });
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
