package com.axiommc.api.command.parser;

import java.util.UUID;

public class UUIDArgParser implements ArgParser<UUID> {

    @Override
    public UUID parse(String input) throws ArgParseException {
        try {
            return UUID.fromString(input);
        } catch (IllegalArgumentException e) {
            throw new ArgParseException("Expected a UUID, got: " + input, e);
        }
    }
}
