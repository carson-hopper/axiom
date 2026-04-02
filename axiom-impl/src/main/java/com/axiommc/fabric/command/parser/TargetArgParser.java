package com.axiommc.fabric.command.parser;

import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.fabric.Axiom;
import java.util.ArrayList;
import java.util.List;

public class TargetArgParser implements ArgParser<String> {

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

    @Override
    public String parse(String input) {
        return input;
    }

    @Override
    public List<String> suggest(String partial) {
        List<String> suggestions = new ArrayList<>();

        // Suggest filters
        for (String filter : FILTER_OPTIONS) {
            if (filter.startsWith(partial)) {
                suggestions.add(filter);
            }
        }

        // Suggest player names (case-insensitive prefix match)
        String lowerPartial = partial.toLowerCase();
        Axiom.players().forEach(player -> {
            String name = player.name();
            if (name.toLowerCase().startsWith(lowerPartial)) {
                suggestions.add(name);
            }
        });

        return suggestions;
    }
}
