package com.axiommc.fabric.plugin;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.plugin.AxiomPlugin;
import com.axiommc.api.plugin.Plugin;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayerProvider;

import java.io.File;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class SimplePluginLoader {

    private final EventBus eventBus;
    private final FabricPlayerProvider playerProvider;
    private final Map<String, AxiomPlugin> plugins = new LinkedHashMap<>();
    private final Map<AxiomPlugin, Boolean> enabledStatus = new HashMap<>();
    private final List<URLClassLoader> classLoaders = new ArrayList<>();
    private final List<PluginResult> loadResults = new ArrayList<>();

    public SimplePluginLoader(EventBus eventBus, FabricPlayerProvider playerProvider) {
        this.eventBus = eventBus;
        this.playerProvider = playerProvider;
    }

    public void loadPlugin(Class<?> pluginClass) {
        if (!AxiomPlugin.class.isAssignableFrom(pluginClass)) {
            loadResults.add(new PluginResult(pluginClass.getSimpleName(), "1.0.0", false));
            return;
        }

        Plugin annotation = pluginClass.getAnnotation(Plugin.class);
        if (annotation == null) {
            loadResults.add(new PluginResult(pluginClass.getSimpleName(), "1.0.0", false));
            return;
        }

        try {
            AxiomPlugin plugin = (AxiomPlugin) pluginClass.getDeclaredConstructor().newInstance();
            plugin.enable(new SimplePluginContext(annotation.id(), annotation.name(), eventBus, playerProvider));
            plugins.put(annotation.id(), plugin);
            enabledStatus.put(plugin, true);
            loadResults.add(new PluginResult(annotation.name(), annotation.version(), true));
        } catch (Exception e) {
            loadResults.add(new PluginResult(annotation.name(), annotation.version(), false));
            Axiom.logger().error("Failed to load plugin: {}", annotation.name(), e);
        }
    }

    public void loadPlugin(File pluginFile) {
        if (!pluginFile.exists()) {
            loadResults.add(new PluginResult(pluginFile.getName(), "?", false));
            return;
        }

        URLClassLoader loader = null;
        try {
            loader = new URLClassLoader(
                    new URL[]{pluginFile.toURI().toURL()},
                    Thread.currentThread().getContextClassLoader()
            );
            classLoaders.add(loader);

            try (ZipFile zipFile = new ZipFile(pluginFile)) {
                for (ZipEntry entry : Collections.list(zipFile.entries())) {
                    if (!entry.getName().endsWith(".class")) {
                        continue;
                    }

                    String className = entry.getName()
                            .replace("/", ".")
                            .replace(".class", "");

                    try {
                        Class<?> clazz = loader.loadClass(className);
                        if (clazz.getAnnotation(Plugin.class) != null
                                && AxiomPlugin.class.isAssignableFrom(clazz)) {
                            loadPlugin(clazz);
                        }
                    } catch (ClassNotFoundException e) {
                        // Skip classes that can't be loaded
                    }
                }
            }
        } catch (Exception e) {
            loadResults.add(new PluginResult(pluginFile.getName(), "?", false));
            Axiom.logger().error("Failed to load plugin JAR: {}", pluginFile.getName(), e);
            if (loader != null) {
                classLoaders.remove(loader);
                try {
                    loader.close();
                } catch (Exception ignored) {}
            }
        }
    }

    /**
     * Prints a formatted summary of all loaded plugins.
     */
    public void printLoadSummary() {
        if (loadResults.isEmpty()) {
            Axiom.logger().info("No plugins loaded");
            return;
        }

        int maxNameLen = 0;
        for (PluginResult result : loadResults) {
            String entry = result.name + " v" + result.version;
            if (entry.length() > maxNameLen) {
                maxNameLen = entry.length();
            }
        }

        int tableWidth = Math.max(maxNameLen + 12, 40);
        String separator = "=".repeat(tableWidth);

        Axiom.logger().info(ChatComponent.text(separator).color(ChatColor.DARK_GRAY));

        for (PluginResult result : loadResults) {
            String entry = result.name + " v" + result.version;
            int padding = tableWidth - entry.length() - 8;
            String pad = " ".repeat(Math.max(1, padding));

            if (result.loaded) {
                Axiom.logger().info(
                        ChatComponent.text("  " + entry + pad).color(ChatColor.WHITE)
                                .append(ChatComponent.text("LOADED").color(ChatColor.GREEN))
                );
            } else {
                Axiom.logger().info(
                        ChatComponent.text("  " + entry + pad).color(ChatColor.WHITE)
                                .append(ChatComponent.text("FAILED").color(ChatColor.RED))
                );
            }
        }

        Axiom.logger().info(ChatComponent.text(separator).color(ChatColor.DARK_GRAY));
    }

    public Optional<AxiomPlugin> plugin(String id) {
        return Optional.ofNullable(plugins.get(id));
    }

    public List<AxiomPlugin> loadedPlugins() {
        return new ArrayList<>(plugins.values());
    }

    public void disablePlugin(AxiomPlugin plugin) {
        if (!enabledStatus.getOrDefault(plugin, false)) {
            return;
        }
        try {
            plugin.onDisable();
            enabledStatus.put(plugin, false);
        } catch (Exception e) {
            Axiom.logger().error("Failed to disable plugin", e);
        }
    }

    public void disableAllPlugins() {
        plugins.values().forEach(this::disablePlugin);
    }

    private record PluginResult(String name, String version, boolean loaded) {}
}
