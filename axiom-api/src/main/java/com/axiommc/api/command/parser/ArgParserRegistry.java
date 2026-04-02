package com.axiommc.api.command.parser;

import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

public class ArgParserRegistry {

    private final Map<Class<?>, ArgParser<?>> parsers = new ConcurrentHashMap<>();

    public ArgParserRegistry() {
        register(String.class, new StringArgParser());
        register(Integer.class, new IntArgParser());
        register(int.class, new IntArgParser());
        register(Long.class, new LongArgParser());
        register(long.class, new LongArgParser());
        register(Double.class, new DoubleArgParser());
        register(double.class, new DoubleArgParser());
        register(Float.class, new FloatArgParser());
        register(float.class, new FloatArgParser());
        register(Boolean.class, new BooleanArgParser());
        register(boolean.class, new BooleanArgParser());
        register(UUID.class, new UUIDArgParser());
    }

    public <T> void register(Class<T> type, ArgParser<T> parser) {
        parsers.put(type, parser);
    }

    /**
     * Returns the parser for the given type. If no parser is registered
     * and the type is an enum, an {@link EnumArgParser} is created and
     * cached automatically.
     */
    @SuppressWarnings("unchecked")
    public <T> ArgParser<T> get(Class<T> type) {
        ArgParser<T> parser = (ArgParser<T>) parsers.get(type);
        if (parser == null && type.isEnum()) {
            parser = createEnumParser(type);
            parsers.put(type, parser);
        }
        return parser;
    }

    @SuppressWarnings({"unchecked", "rawtypes"})
    private <T> ArgParser<T> createEnumParser(Class<T> type) {
        return (ArgParser<T>) new EnumArgParser<>((Class<Enum>) type);
    }

    public boolean has(Class<?> type) {
        return parsers.containsKey(type);
    }
}
