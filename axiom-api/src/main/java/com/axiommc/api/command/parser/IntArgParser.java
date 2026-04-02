package com.axiommc.api.command.parser;

public class IntArgParser implements ArgParser<Integer> {

    @Override
    public Integer parse(String input) throws ArgParseException {
        try {
            return Integer.parseInt(input);
        } catch (NumberFormatException e) {
            throw new ArgParseException("Expected an integer, got: " + input, e);
        }
    }
}
