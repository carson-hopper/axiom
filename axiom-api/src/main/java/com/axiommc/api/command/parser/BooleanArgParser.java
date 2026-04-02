package com.axiommc.api.command.parser;

import java.util.List;

public class BooleanArgParser implements ArgParser<Boolean> {

    @Override
    public Boolean parse(String input) throws ArgParseException {
        return switch (input.toLowerCase()) {
            case "true", "yes", "on", "1" -> true;
            case "false", "no", "off", "0" -> false;
            default -> throw new ArgParseException("Expected true/false, got: " + input);
        };
    }

    @Override
    public List<String> suggest(String partial) {
        return List.of("true", "false");
    }
}
