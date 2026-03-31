package com.axiommc.fabric.command.parser;

import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;

import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;

public class FabricPlayerArgParser implements ArgParser<Player> {

    @Override
    public Player parse(String arg) {
        Optional<FabricPlayer> player = Axiom.player(arg);
        if (player.isPresent()) {
            return player.get();
        }

        try {
            return Axiom.player(UUID.fromString(arg)).orElse(null);
        } catch (Exception _) {

        }
        return null;
    }

    @Override
    public List<String> suggest(String partial) {
        String lowerPartial = partial.toLowerCase();
        return Axiom.players().stream()
                .map(Player::name)
                .filter(name -> name.toLowerCase().startsWith(lowerPartial))
                .collect(Collectors.toList());
    }
}
