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

    @SuppressWarnings("unchecked")
    public <T> ArgParser<T> get(Class<T> type) {
        return (ArgParser<T>) parsers.get(type);
    }

    public boolean has(Class<?> type) {
        return parsers.containsKey(type);
    }
}
