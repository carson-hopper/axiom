package com.axiommc.api.command.parser;

import java.util.Collections;
import java.util.List;

public interface ArgParser<T> {

    T parse(String input) throws ArgParseException;

    default List<String> suggest(String partial) {
        return Collections.emptyList();
    }
}
