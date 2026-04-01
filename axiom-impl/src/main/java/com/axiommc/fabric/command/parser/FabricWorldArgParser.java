package com.axiommc.fabric.command.parser;

import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.api.command.parser.ArgParseException;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;

import java.util.List;
import java.util.stream.Collectors;

public class FabricWorldArgParser implements ArgParser<World> {

    @Override
    public World parse(String arg) throws ArgParseException {
        String lowerArg = arg.toLowerCase();

        for (World world : Axiom.worlds()) {
            if (world.name().replace("minecraft:", "").toLowerCase().equals(lowerArg)) {
                return world;
            }
        }

        throw new ArgParseException("World not found: " + arg);
    }

    @Override
    public List<String> suggest(String partial) {
        String lowerPartial = partial.toLowerCase();
        return Axiom.worlds().stream()
                .map(world -> world.name().replace("minecraft:", ""))
                .filter(name -> name.toLowerCase().startsWith(lowerPartial))
                .collect(Collectors.toList());
    }
}
