package com.axiommc.api.command.parser;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Generic argument parser for any enum type. Matches enum constant
 * names case-insensitively. Tab completion suggests all constants.
 *
 * @param <E> the enum type
 */
public class EnumArgParser<E extends Enum<E>> implements ArgParser<E> {

    private final Class<E> enumClass;

    public EnumArgParser(Class<E> enumClass) {
        this.enumClass = enumClass;
    }

    @Override
    public E parse(String input) throws ArgParseException {
        for (E constant : enumClass.getEnumConstants()) {
            if (constant.name().equalsIgnoreCase(input)) {
                return constant;
            }
        }
        String valid = Arrays.stream(enumClass.getEnumConstants())
                .map(constant -> constant.name().toLowerCase())
                .collect(Collectors.joining(", "));
        throw new ArgParseException(
                "Unknown " + enumClass.getSimpleName() + ": " + input
                        + ". Valid values: " + valid);
    }

    @Override
    public List<String> suggest(String partial) {
        String lowerPartial = partial.toLowerCase();
        return Arrays.stream(enumClass.getEnumConstants())
                .map(constant -> constant.name().toLowerCase())
                .filter(name -> name.startsWith(lowerPartial))
                .collect(Collectors.toList());
    }
}
