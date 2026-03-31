package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.command.filter.TargetFilter;
import java.util.ArrayList;
import java.util.List;

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

    @Subcommand
    public void player(CommandSender sender, @Arg("target") @DynamicTabComplete("suggestTargets") String target) {
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

        // Suggest filters
        String lowerPartial = partial.toLowerCase();
        for (String filter : FILTER_OPTIONS) {
            if (filter.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(filter);
            }
        }

        // Suggest player names
        Axiom.players().forEach(player -> {
            if (player.name().toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(player.name());
            }
        });

        return suggestions;
    }
}
