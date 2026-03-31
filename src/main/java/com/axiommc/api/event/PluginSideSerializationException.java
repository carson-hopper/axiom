package com.axiommc.api.event;

/**
 * Thrown at publish time when a {@code @CrossSide} event cannot be serialized.
 * Common causes: unsupported field type, or missing no-arg constructor.
 */
public class PluginSideSerializationException extends RuntimeException {

    public PluginSideSerializationException(String message) {
        super(message);
    }
}
