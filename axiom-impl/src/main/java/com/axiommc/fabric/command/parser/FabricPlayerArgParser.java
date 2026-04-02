package com.axiommc.fabric.command.parser;

import com.axiommc.api.command.parser.ArgParseException;
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
    public Player parse(String arg) throws ArgParseException {
        Optional<FabricPlayer> player = Axiom.player(arg);
        if (player.isPresent()) {
            return player.get();
        }

        try {
            Optional<FabricPlayer> byUuid = Axiom.player(UUID.fromString(arg));
            if (byUuid.isPresent()) {
                return byUuid.get();
            }
        } catch (IllegalArgumentException _) {
            // Not a valid UUID format
        }

        throw new ArgParseException("Player not found: " + arg);
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
