package com.axiommc.api.command.parser;

public class LongArgParser implements ArgParser<Long> {

    @Override
    public Long parse(String input) throws ArgParseException {
        try {
            return Long.parseLong(input);
        } catch (NumberFormatException e) {
            throw new ArgParseException("Expected a long, got: " + input, e);
        }
    }

}
