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

    private static final int LINE_WIDTH = 40;

    private final EventBus eventBus;
    private final FabricPlayerProvider playerProvider;
    private final Map<String, AxiomPlugin> plugins = new LinkedHashMap<>();
    private final Map<AxiomPlugin, Boolean> enabledStatus = new HashMap<>();
    private final List<URLClassLoader> classLoaders = new ArrayList<>();
    private final List<PluginEntry> entries = new ArrayList<>();

    public SimplePluginLoader(EventBus eventBus, FabricPlayerProvider playerProvider) {
        this.eventBus = eventBus;
        this.playerProvider = playerProvider;
    }

    public void loadPlugin(Class<?> pluginClass) {
        if (!AxiomPlugin.class.isAssignableFrom(pluginClass)) {
            entries.add(new PluginEntry(pluginClass.getSimpleName(), "?"));
            updateStatus(pluginClass.getSimpleName(), PluginState.FAILED);
            return;
        }

        Plugin annotation = pluginClass.getAnnotation(Plugin.class);
        if (annotation == null) {
            entries.add(new PluginEntry(pluginClass.getSimpleName(), "?"));
            updateStatus(pluginClass.getSimpleName(), PluginState.FAILED);
            return;
        }

        String displayName = annotation.name();
        PluginEntry entry = new PluginEntry(displayName, annotation.version());
        entries.add(entry);

        updateStatus(displayName, PluginState.LOADING);

        try {
            AxiomPlugin plugin = (AxiomPlugin) pluginClass.getDeclaredConstructor().newInstance();

            updateStatus(displayName, PluginState.ENABLING);

            plugin.enable(new SimplePluginContext(annotation.id(), annotation.name(), eventBus, playerProvider));
            plugins.put(annotation.id(), plugin);
            enabledStatus.put(plugin, true);

            updateStatus(displayName, PluginState.LOADED);
        } catch (Exception e) {
            updateStatus(displayName, PluginState.FAILED);
            Axiom.logger().error("Failed to load plugin: %s", displayName, e);
        }
    }

    public void loadPlugin(File pluginFile) {
        if (!pluginFile.exists()) {
            entries.add(new PluginEntry(pluginFile.getName(), "?"));
            updateStatus(pluginFile.getName(), PluginState.FAILED);
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
                for (ZipEntry zipEntry : Collections.list(zipFile.entries())) {
                    if (!zipEntry.getName().endsWith(".class")) {
                        continue;
                    }

                    String className = zipEntry.getName()
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
            entries.add(new PluginEntry(pluginFile.getName(), "?"));
            updateStatus(pluginFile.getName(), PluginState.FAILED);
            Axiom.logger().error("Failed to load plugin JAR: %s", pluginFile.getName(), e);
            if (loader != null) {
                classLoaders.remove(loader);
                try {
                    loader.close();
                } catch (Exception ignored) {}
            }
        }
    }

    /**
     * Prints a formatted summary of all loaded plugins in Maven-style output.
     */
    /**
     * Prints the opening separator before plugins start loading.
     */
    public void printLoadHeader() {
        Axiom.logger().info(ChatComponent.text("-".repeat(LINE_WIDTH)).color(ChatColor.DARK_GRAY));
    }

    /**
     * Prints the closing separator after all plugins are loaded.
     */
    public void printLoadSummary() {
        if (entries.isEmpty()) {
            Axiom.logger().info("No plugins loaded");
            return;
        }
        Axiom.logger().info(ChatComponent.text("-".repeat(LINE_WIDTH)).color(ChatColor.DARK_GRAY));
    }

    private void printEntry(PluginEntry entry) {
        String name = entry.name + " v" + entry.version;
        String status = entry.state.label;
        int dotsLen = LINE_WIDTH - name.length() - status.length() - 2;
        String dots = " " + ".".repeat(Math.max(1, dotsLen)) + " ";

        Axiom.logger().info(
                ChatComponent.text(name).color(ChatColor.WHITE)
                        .append(ChatComponent.text(dots).color(ChatColor.DARK_GRAY))
                        .append(ChatComponent.text(status).color(entry.state.color))
        );
    }

    private void updateStatus(String name, PluginState state) {
        for (PluginEntry entry : entries) {
            if (entry.name.equals(name)) {
                entry.state = state;
                if (state == PluginState.LOADED || state == PluginState.FAILED) {
                    printEntry(entry);
                }
                return;
            }
        }
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

    private enum PluginState {
        LOADING("LOADING", ChatColor.YELLOW),
        ENABLING("ENABLING", ChatColor.GOLD),
        LOADED("LOADED", ChatColor.GREEN),
        FAILED("FAILED", ChatColor.RED);

        final String label;
        final ChatColor color;

        PluginState(String label, ChatColor color) {
            this.label = label;
            this.color = color;
        }
    }

    private static class PluginEntry {
        final String name;
        final String version;
        PluginState state;

        PluginEntry(String name, String version) {
            this.name = name;
            this.version = version;
            this.state = PluginState.LOADING;
        }
    }
}
