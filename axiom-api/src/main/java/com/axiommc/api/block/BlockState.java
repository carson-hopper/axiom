package com.axiommc.api.block;

import java.util.Map;

/**
 * The property values of a placed block (e.g. {@code facing=north}, {@code powered=true}).
 * Keys and values are the Minecraft serialisation strings ("facing", "north", "true", "3", …).
 */
public record BlockState(Map<String, String> properties) {

    public static final BlockState EMPTY = new BlockState(Map.of());

    public BlockState(Map<String, String> properties) {
        this.properties = Map.copyOf(properties);
    }

    /** Returns the value for the given property key, or {@code null} if absent. */
    public String property(String key) {
        return properties.get(key);
    }

    public boolean hasProperty(String key) {
        return properties.containsKey(key);
    }
}
