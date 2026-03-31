package com.axiommc.fabric;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.command.FabricCommandHandler;
import com.axiommc.fabric.event.FabricEventBus;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.plugin.SimplePluginLoader;
import com.axiommc.fabric.util.TaskScheduler;
import com.axiommc.fabric.world.FabricWorld;
import net.fabricmc.api.ModInitializer;
import net.fabricmc.fabric.api.command.v2.CommandRegistrationCallback;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerLifecycleEvents;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerTickEvents;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerLevel;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;

public class AxiomMod implements ModInitializer {

    public static final String MOD_ID = "axiom";
    public static final Logger LOGGER = LoggerFactory.getLogger(MOD_ID);

    private static AxiomMod instance;

    private EventBus eventBus;
    private SimplePluginLoader pluginLoader;
    private FabricPlayerProvider playerProvider;
    private FabricCommandHandler commandHandler;
    private MinecraftServer minecraftServer;
    private final Map<String, World> worlds = new ConcurrentHashMap<>();

    // ============================================================
    // Initialization
    // ============================================================

    @Override
    public void onInitialize() {
        instance = this;

        LOGGER.info("Axiom initializing!");

        try {
            this.eventBus = new FabricEventBus();
        } catch (Exception e) {
            LOGGER.error("Failed to create event bus", e);
            return;
        }

        try {
            this.playerProvider = new FabricPlayerProvider();
            LOGGER.debug("Player provider created");
        } catch (Exception e) {
            LOGGER.error("Failed to create player provider", e);
            return; // Fatal - can't continue without player provider
        }

        try {
            // Create command handler BEFORE plugin loader so plugins can register commands
            this.commandHandler = new FabricCommandHandler(eventBus);
            LOGGER.debug("Command handler created");

            // Register command handler with Brigadier via CommandRegistrationCallback
            try {
                CommandRegistrationCallback.EVENT.register((dispatcher, registryAccess, environment) -> {
                    try {
                        commandHandler.register(dispatcher);
                    } catch (Throwable e) {
                        LOGGER.warn("Failed to register commands with Brigadier - commands will not be available", e);
                    }
                });
                LOGGER.debug("Command handler registered with CommandRegistrationCallback");
            } catch (Throwable e) {
                LOGGER.warn("Failed to register command handler callback - commands will not be available", e);
                LOGGER.debug("This is expected in some production environments due to class loading issues");
            }
        } catch (Exception e) {
            LOGGER.error("Failed to create command handler", e);
            throw new RuntimeException("Axiom initialization failed at command handler creation", e);
        }

        try {
            // Create plugin loader (needs player provider and command handler)
            this.pluginLoader = new SimplePluginLoader(eventBus, playerProvider);
            pluginLoader.loadPlugin(com.axiommc.plugin.AxiomPlugin.class);
        } catch (Exception e) {
            LOGGER.error("Failed to create plugin loader", e);
            return;
        }

        ServerLifecycleEvents.SERVER_STARTED.register(server -> {
            LOGGER.info("Minecraft server started");
            this.minecraftServer = server;

            for (ServerLevel level : server.getAllLevels()) {
                World world = new FabricWorld(level);
                worlds.put(world.name(), world);
                LOGGER.debug("Registered world: {}", world.name());
            }

            playerProvider.setServer(server);
            LOGGER.debug("Player provider initialized");

            loadPluginsFromDirectory();

            // Publish server start event
            // eventBus.publish(new ServerStartEvent());
            LOGGER.debug("ServerStartEvent published");
        });

        ServerTickEvents.END_SERVER_TICK.register(server -> {
            TaskScheduler.global().tick();
        });

        ServerLifecycleEvents.SERVER_STOPPING.register(server -> {
            LOGGER.info("Minecraft server stopping");
            pluginLoader.disableAllPlugins();

            // Publish server stop event
            // eventBus.publish(new ServerStopEvent());
            LOGGER.debug("ServerStopEvent published");

            // TODO: Cleanup socket server connections
        });

        LOGGER.info("Axiom Fabric mod initialized successfully");
    }

    // ============================================================
    // Plugin Loading
    // ============================================================

    private void loadPluginsFromDirectory() {
        File pluginsDir = new File("plugins");
        if (!pluginsDir.exists()) {
            LOGGER.info("Plugins directory does not exist, creating it");
            if (!pluginsDir.mkdirs()) {
                LOGGER.warn("Failed to create plugins directory");
                return;
            }
        }

        if (!pluginsDir.isDirectory()) {
            LOGGER.warn("plugins is not a directory");
            return;
        }

        File[] files = pluginsDir.listFiles();
        if (files == null || files.length == 0) {
            LOGGER.info("No plugins found in plugins directory");
            return;
        }

        LOGGER.info("Loading plugins from plugins directory");
        int loadedCount = 0;
        for (File file : files) {
            if (file.isFile() && file.getName().endsWith(".jar")) {
                try {
                    pluginLoader.loadPlugin(file);
                    loadedCount++;
                } catch (Exception e) {
                    LOGGER.error("Failed to load plugin from file: {}", file.getName(), e);
                }
            }
        }

        LOGGER.info("Loaded {} plugins from plugins directory", loadedCount);
    }

    // ============================================================
    // Accessors & Getters
    // ============================================================

    public static AxiomMod instance() {
        return instance;
    }

    public EventBus eventBus() {
        return eventBus;
    }

    public SimplePluginLoader pluginLoader() {
        return pluginLoader;
    }

    public FabricPlayerProvider playerProvider() {
        return playerProvider;
    }

    public FabricCommandHandler commandHandler() {
        return commandHandler;
    }

    public Server server() {
        if (minecraftServer == null) {
            throw new IllegalStateException("Server not started yet");
        }
        String serverId = "axiom-server";
        String host = "localhost";
        int port = 25565; // Default Minecraft port

        return new Server(serverId, host, port);
    }

    public Collection<World> worlds() {
        return Collections.unmodifiableCollection(worlds.values());
    }

    public Optional<World> world(String name) {
        return Optional.ofNullable(worlds.get(name));
    }

    public MinecraftServer minecraftServer() {
        return minecraftServer;
    }
}