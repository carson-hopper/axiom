package com.axiommc.fabric.plugin;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.plugin.AxiomPlugin;
import com.axiommc.api.plugin.Plugin;
import com.axiommc.fabric.player.FabricPlayerProvider;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

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

    private static final Logger LOGGER = LoggerFactory.getLogger(SimplePluginLoader.class);

    private final EventBus eventBus;
    private final FabricPlayerProvider playerProvider;
    private final Map<String, AxiomPlugin> plugins = new LinkedHashMap<>();
    private final Map<AxiomPlugin, Boolean> enabledStatus = new HashMap<>();
    private final List<URLClassLoader> classLoaders = new ArrayList<>();

    public SimplePluginLoader(EventBus eventBus, FabricPlayerProvider playerProvider) {
        this.eventBus = eventBus;
        this.playerProvider = playerProvider;
    }

    public void loadPlugin(Class<?> pluginClass) {
        if (!AxiomPlugin.class.isAssignableFrom(pluginClass)) {
            LOGGER.warn("Class {} does not implement AxiomPlugin", pluginClass.getName());
            return;
        }

        Plugin annotation = pluginClass.getAnnotation(Plugin.class);
        if (annotation == null) {
            LOGGER.warn("Class {} is missing @Plugin annotation", pluginClass.getName());
            return;
        }

        try {
            AxiomPlugin plugin = (AxiomPlugin) pluginClass.getDeclaredConstructor().newInstance();

            plugin.enable(new SimplePluginContext(annotation.id(), annotation.name(), eventBus, playerProvider));
            plugins.put(annotation.id(), plugin);
            enabledStatus.put(plugin, true);

            LOGGER.info("Loaded plugin: {} v{}", annotation.name(), annotation.version());
        } catch (Exception e) {
            LOGGER.error("Failed to load plugin from class {}", pluginClass.getName(), e);
        }
    }

    public void loadPlugin(File pluginFile) {
        if (!pluginFile.exists()) {
            LOGGER.warn("Plugin file does not exist: {}", pluginFile.getAbsolutePath());
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

                        if (clazz.getAnnotation(Plugin.class) != null &&
                                AxiomPlugin.class.isAssignableFrom(clazz)) {
                            loadPlugin(clazz);
                        }
                    } catch (ClassNotFoundException e) {
                        LOGGER.trace("Could not load class from JAR: {}", className, e);
                    }
                }

                LOGGER.info("Scanned plugin JAR: {}", pluginFile.getName());
            }
        } catch (Exception e) {
            LOGGER.error("Failed to load plugins from JAR: {}", pluginFile.getName(), e);
            if (loader != null) {
                classLoaders.remove(loader);
                try {
                    loader.close();
                } catch (Exception ignored) {}
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
            return; // Not enabled
        }
        try {
            plugin.onDisable();
            enabledStatus.put(plugin, false);
            LOGGER.info("Disabled plugin");
        } catch (Exception e) {
            LOGGER.error("Failed to disable plugin", e);
        }
    }

    public void disableAllPlugins() {
        plugins.values().forEach(this::disablePlugin);
    }
}
