package com.axiommc.api.command.parser;

public class StringArgParser implements ArgParser<String> {

    @Override
    public String parse(String input) {
        return input;
    }
}
