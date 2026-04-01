package com.axiommc.api.command.parser;

public class FloatArgParser implements ArgParser<Float> {

    @Override
    public Float parse(String input) throws ArgParseException {
        try {
            return Float.parseFloat(input);
        } catch (NumberFormatException e) {
            throw new ArgParseException("Expected a float, got: " + input, e);
        }
    }

}
