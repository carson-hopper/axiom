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
import java.util.List;
import java.util.Set;

@CommandMeta(
        name = "kill",
        description = "Kill a player or mob",
        permission = "axiom.kill"
)
public class KillCommand implements Command {

    private static final String[] FILTER_OPTIONS = {
        "filter:players",
        "filter:mobs",
        "filter:entities",
        "filter:hostile",
        "filter:passive",
        "filter:animals",
        "filter:all",
        "filter:!self",
        "filter:!players"
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
        var targets = TargetFilter.parse(target, sender);

        if (targets.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No targets found: " + target).color(ChatColor.RED));
            return;
        }

        for (LivingEntity entity : targets) {
            entity.damage(entity.health());
        }

        if (targets.size() == 1) {
            sender.sendMessage(ChatComponent.text("Killed " + targets.get(0).name()).color(ChatColor.RED));
        } else {
            sender.sendMessage(ChatComponent.text("Killed " + targets.size() + " entities").color(ChatColor.RED));
        }
    }

    public List<String> suggestTargets(String partial) {
        List<String> suggestions = new ArrayList<>();
        String lowerPartial = partial.toLowerCase();

        // Suggest filter keywords
        for (String filter : FILTER_OPTIONS) {
            if (filter.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filter);
            }
        }

        // Suggest filter:PlayerName syntax
        Axiom.players().forEach(player -> {
            String filterName = "filter:" + player.name();
            if (filterName.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filterName);
            }
        });

        // Suggest filter:!PlayerName syntax
        Axiom.players().forEach(player -> {
            String filterName = "filter:!" + player.name();
            if (filterName.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filterName);
            }
        });

        // Discover and suggest mob types in world
        Set<String> mobTypes = discoverMobTypes();
        mobTypes.forEach(mobType -> {
            String filterName = "filter:" + mobType;
            if (filterName.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filterName);
            }
        });

        // Suggest filter:!MobType syntax
        mobTypes.forEach(mobType -> {
            String filterName = "filter:!" + mobType;
            if (filterName.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filterName);
            }
        });

        // Suggest direct player names
        Axiom.players().forEach(player -> {
            if (player.name().toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(player.name());
            }
        });

        return suggestions;
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
