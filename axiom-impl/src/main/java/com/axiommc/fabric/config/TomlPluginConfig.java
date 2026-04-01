package com.axiommc.fabric.config;

import com.axiommc.api.config.PluginConfig;
import com.moandjiezana.toml.Toml;
import com.moandjiezana.toml.TomlWriter;
import com.axiommc.fabric.Axiom;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class TomlPluginConfig implements PluginConfig {

    private final File configFile;
    private Map<String, Object> data;

    public TomlPluginConfig(File configFile) {
        this.configFile = configFile;
        this.data = new LinkedHashMap<>();
        reload();
    }

    // ============================================================
    // Configuration Getters
    // ============================================================

    @Override
    public String getString(String key, String defaultValue) {
        try {
            Object value = data.get(key);
            if (value instanceof String) {
                return (String) value;
            }
            return defaultValue;
        } catch (Exception e) {
            Axiom.logger().warn("Error getting string value for key {}", key, e);
            return defaultValue;
        }
    }

    @Override
    public int getInt(String key, int defaultValue) {
        try {
            Object value = data.get(key);
            if (value instanceof Long) {
                return ((Long) value).intValue();
            }
            return defaultValue;
        } catch (Exception e) {
            Axiom.logger().warn("Error getting int value for key {}", key, e);
            return defaultValue;
        }
    }

    @Override
    public boolean getBoolean(String key, boolean defaultValue) {
        try {
            Object value = data.get(key);
            if (value instanceof Boolean) {
                return (Boolean) value;
            }
            return defaultValue;
        } catch (Exception e) {
            Axiom.logger().warn("Error getting boolean value for key {}", key, e);
            return defaultValue;
        }
    }

    @Override
    public double getDouble(String key, double defaultValue) {
        try {
            Object value = data.get(key);
            if (value instanceof Double) {
                return (Double) value;
            }
            if (value instanceof Long) {
                return ((Long) value).doubleValue();
            }
            return defaultValue;
        } catch (Exception e) {
            Axiom.logger().warn("Error getting double value for key {}", key, e);
            return defaultValue;
        }
    }

    @Override
    public List<String> getStringList(String key) {
        try {
            Object value = data.get(key);
            if (value instanceof List) {
                List<String> result = new ArrayList<>();
                for (Object item : (List<?>) value) {
                    if (item != null) {
                        result.add(item.toString());
                    }
                }
                return result;
            }
            return new ArrayList<>();
        } catch (Exception e) {
            Axiom.logger().warn("Error getting list value for key {}", key, e);
            return new ArrayList<>();
        }
    }

    // ============================================================
    // Configuration Management
    // ============================================================

    @Override
    public void set(String key, Object value) {
        data.put(key, value);
    }

    @Override
    public void save() {
        try {
            // Ensure parent directory exists
            if (configFile.getParentFile() != null) {
                configFile.getParentFile().mkdirs();
            }

            // Write TOML to file
            TomlWriter writer = new TomlWriter();
            try (FileWriter fileWriter = new FileWriter(configFile)) {
                writer.write(data, fileWriter);
            }
            Axiom.logger().debug("Saved config to {}", configFile.getAbsolutePath());
        } catch (IOException e) {
            Axiom.logger().warn("Failed to save config to {}", configFile.getAbsolutePath(), e);
        }
    }

    @Override
    public void reload() {
        try {
            if (!configFile.exists()) {
                createDefaultConfig();
            }

            if (configFile.exists()) {
                Toml toml = new Toml().read(configFile);
                data = new LinkedHashMap<>(toml.toMap());
                Axiom.logger().debug("Loaded config from {}", configFile.getAbsolutePath());
            } else {
                data = new LinkedHashMap<>();
                Axiom.logger().debug("Config file {} does not exist, using empty config", configFile.getAbsolutePath());
            }
        } catch (Exception e) {
            Axiom.logger().warn("Failed to load config from {}", configFile.getAbsolutePath(), e);
            data = new LinkedHashMap<>();
        }
    }

    private void createDefaultConfig() {
        try {
            if (configFile.getParentFile() != null) {
                configFile.getParentFile().mkdirs();
            }

            InputStream defaultConfig = getClass().getResourceAsStream("/default-config.toml");
            if (defaultConfig != null) {
                Files.copy(defaultConfig, configFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
                Axiom.logger().info("Created default config at {}", configFile.getAbsolutePath());
            } else {
                Axiom.logger().warn("Default config template not found in resources");
            }
        } catch (IOException e) {
            Axiom.logger().warn("Failed to create default config at {}", configFile.getAbsolutePath(), e);
        }
    }

    // ============================================================
    // Query Methods
    // ============================================================

    @Override
    public boolean contains(String key) {
        return data.containsKey(key);
    }
}
