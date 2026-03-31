package com.axiommc.api.command.parser;

public class DoubleArgParser implements ArgParser<Double> {

    @Override
    public Double parse(String input) throws ArgParseException {
        try {
            return Double.parseDouble(input);
        } catch (NumberFormatException e) {
            throw new ArgParseException("Expected a double, got: " + input, e);
        }
    }

}
