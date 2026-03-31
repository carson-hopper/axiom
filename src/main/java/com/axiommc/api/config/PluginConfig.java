package com.axiommc.api.config;

import java.util.List;

/**
 * Plugin configuration interface for reading and writing TOML configuration files.
 */
public interface PluginConfig {

    /**
     * Get a string value from config, or return default if not present.
     */
    String getString(String key, String defaultValue);

    /**
     * Get an integer value from config, or return default if not present.
     */
    int getInt(String key, int defaultValue);

    /**
     * Get a boolean value from config, or return default if not present.
     */
    boolean getBoolean(String key, boolean defaultValue);

    /**
     * Get a double value from config, or return default if not present.
     */
    double getDouble(String key, double defaultValue);

    /**
     * Get a list of strings from config, or empty list if not present.
     */
    List<String> getStringList(String key);

    /**
     * Set a value in the config (in memory).
     */
    void set(String key, Object value);

    /**
     * Save configuration to file.
     */
    void save();

    /**
     * Reload configuration from file.
     */
    void reload();

    /**
     * Check if config contains a key.
     */
    boolean contains(String key);
}
