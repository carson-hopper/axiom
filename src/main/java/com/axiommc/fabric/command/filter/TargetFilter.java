package com.axiommc.fabric.command.filter;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

public class TargetFilter {

    public static List<Player> parse(String input, CommandSender sender) {
        if (input.startsWith("filter:")) {
            return parseFilter(input.substring(7), sender);
        }
        return parsePlayer(input, sender);
    }

    private static List<Player> parsePlayer(String playerName, CommandSender sender) {
        var player = Axiom.players().stream()
                .filter(p -> p.name().equalsIgnoreCase(playerName))
                .findFirst();

        List<Player> result = new ArrayList<>();
        player.ifPresent(result::add);
        return result;
    }

    private static List<Player> parseFilter(String filter, CommandSender sender) {
        List<Player> targets = new ArrayList<>(Axiom.players());

        if (filter.startsWith("!")) {
            String negated = filter.substring(1);
            return applyNegation(targets, negated, sender);
        }

        return switch (filter) {
            case "players", "all" -> targets;
            default -> new ArrayList<>();
        };
    }

    private static List<Player> applyNegation(Collection<Player> targets, String filter, CommandSender sender) {
        return switch (filter) {
            case "self" -> {
                if (sender.isPlayer()) {
                    Player self = sender.asPlayer().get();
                    List<Player> result = new ArrayList<>(targets);
                    result.remove(self);
                    yield result;
                }
                yield new ArrayList<>(targets);
            }
            default -> new ArrayList<>();
        };
    }
}
