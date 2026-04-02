package com.axiommc.fabric;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.server.ServerStartEvent;
import com.axiommc.api.event.server.ServerStopEvent;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.command.FabricCommandHandler;
import com.axiommc.fabric.event.FabricEventBus;
import com.axiommc.fabric.event.adapter.ServerLifecycleAdapter;
import com.axiommc.fabric.gui.FabricGuiManager;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.plugin.SimplePluginLoader;
import com.axiommc.fabric.screen.FabricScreenManager;
import com.axiommc.fabric.sidebar.FabricSidebarManager;
import com.axiommc.fabric.util.TaskScheduler;
import com.axiommc.fabric.world.FabricWorld;
import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.tree.CommandNode;
import java.io.File;
import java.lang.reflect.Field;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import net.fabricmc.api.ModInitializer;
import net.fabricmc.fabric.api.command.v2.CommandRegistrationCallback;
import net.fabricmc.fabric.api.event.lifecycle.v1.ServerTickEvents;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerLevel;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class AxiomMod implements ModInitializer {

    public static final String MOD_ID = "axiom";
    public static final Logger LOGGER = LoggerFactory.getLogger(MOD_ID);

    private static AxiomMod instance;

    private EventBus eventBus;
    private SimplePluginLoader pluginLoader;
    private FabricPlayerProvider playerProvider;
    private FabricCommandHandler commandHandler;
    private MinecraftServer minecraftServer;
    private GuiManager guiManager;
    private SidebarManager sidebarManager;
    private ScreenManager screenManager;
    private final Map<String, World> worlds = new ConcurrentHashMap<>();

    // ============================================================
    // Initialization
    // ============================================================

    @Override
    public void onInitialize() {
        instance = this;

        // Suppress noisy vanilla join/UUID log messages
        com.axiommc.fabric.console.VanillaLogFilter.install();

        // ────────────────────────────────────────────────────────
        // Event Bus Setup
        // ────────────────────────────────────────────────────────

        try {
            this.eventBus = new FabricEventBus();
        } catch (Exception e) {
            Axiom.logger().error("Failed to create event bus", e);
            return;
        }

        // ────────────────────────────────────────────────────────
        // Player Provider Setup
        // ────────────────────────────────────────────────────────

        try {
            this.playerProvider = new FabricPlayerProvider();

            if (eventBus instanceof FabricEventBus fabricEventBus) {
                fabricEventBus.initialize(playerProvider);
            }
        } catch (Exception e) {
            Axiom.logger().error("Failed to create player provider", e);
            return; // Fatal - can't continue without player provider
        }

        // ────────────────────────────────────────────────────────
        // Command Handler Setup
        // ────────────────────────────────────────────────────────

        try {
            this.commandHandler = new FabricCommandHandler(eventBus);

            try {
                CommandRegistrationCallback.EVENT.register(
                    (dispatcher, registryAccess, environment) -> {
                        try {
                            // unregisterAllCommands(dispatcher);
                            commandHandler.register(dispatcher);
                        } catch (Throwable e) {
                            Axiom.logger()
                                .warn(
                                    "Failed to register commands with Brigadier - commands will"
                                        + " not be available",
                                    e);
                        }
                    });
                Axiom.logger().debug("Command handler registered with CommandRegistrationCallback");
            } catch (Throwable e) {
                Axiom.logger()
                    .warn(
                        "Failed to register command handler callback - commands will not be"
                            + " available",
                        e);
                Axiom.logger()
                    .debug("This is expected in some production environments due to class loading"
                        + " issues");
            }
        } catch (Exception e) {
            Axiom.logger().error("Failed to create command handler", e);
            throw new RuntimeException(
                "Axiom initialization failed at command handler creation", e);
        }

        // ────────────────────────────────────────────────────────
        // Plugin Loader Setup
        // ────────────────────────────────────────────────────────

        try {
            this.pluginLoader = new SimplePluginLoader(eventBus, playerProvider);
            pluginLoader.printLoadHeader();
            pluginLoader.loadPlugin(com.axiommc.plugin.AxiomPlugin.class);
            pluginLoader.printLoadSummary();
        } catch (Exception e) {
            Axiom.logger().error("Failed to create plugin loader", e);
            return;
        }

        // ────────────────────────────────────────────────────────
        // Server Lifecycle Events (via event bus)
        // ────────────────────────────────────────────────────────

        ServerTickEvents.END_SERVER_TICK.register(server -> {
            TaskScheduler.global().tick();
            if (screenManager instanceof FabricScreenManager fsm) {
                fsm.tick();
            }
        });

        eventBus.listen(ServerStartEvent.class).handler(event -> {
            this.minecraftServer = ServerLifecycleAdapter.minecraftServer();
            this.guiManager = new FabricGuiManager();
            this.sidebarManager = new FabricSidebarManager(minecraftServer);
            this.screenManager = new FabricScreenManager(eventBus);

            for (ServerLevel level : minecraftServer.getAllLevels()) {
                World world = new FabricWorld(level);
                worlds.put(world.name(), world);
            }

            playerProvider.setServer(minecraftServer);

            loadPluginsFromDirectory();
        });

        eventBus.listen(ServerStopEvent.class).handler(event -> {
            pluginLoader.disableAllPlugins();
        });
    }

    // ============================================================
    // Command Management
    // ============================================================

    /**
     * Unregisters all commands (including vanilla ones) from Brigadier dispatcher.
     *
     * <p>This replaces the entire command tree with a clean slate, allowing Axiom
     * to register only custom commands via {@link FabricCommandHandler}. This is
     * an intentional architectural choice that removes vanilla commands like /tp,
     * /gamemode, and /give.
     *
     * <p>Uses reflection to access private Brigadier fields and clear them. This
     * is fragile and may break with Brigadier updates, but is necessary because
     * Brigadier does not provide a public API for removing registered commands.
     */
    private static void unregisterAllCommands(CommandDispatcher<?> dispatcher) {
        CommandNode<?> root = dispatcher.getRoot();

        try {
            Field childrenField = root.getClass().getSuperclass().getDeclaredField("children");
            Field literalsField = root.getClass().getSuperclass().getDeclaredField("literals");
            Field argumentsField = root.getClass().getSuperclass().getDeclaredField("arguments");

            childrenField.setAccessible(true);
            literalsField.setAccessible(true);
            argumentsField.setAccessible(true);

            ((Map<?, ?>) childrenField.get(root)).clear();
            ((Map<?, ?>) literalsField.get(root)).clear();
            ((Map<?, ?>) argumentsField.get(root)).clear();

        } catch (NoSuchFieldException | IllegalAccessException e) {
            throw new RuntimeException("Failed to unregister commands", e);
        }
    }

    // ============================================================
    // Plugin Loading
    // ============================================================

    // ────────────────────────────────────────────────────────
    // Directory Management
    // ────────────────────────────────────────────────────────

    private void loadPluginsFromDirectory() {
        File pluginsDir = new File("plugins");
        if (!ensurePluginDirectory(pluginsDir)) {
            return;
        }

        File[] files = pluginsDir.listFiles();
        if (files == null) {
            return;
        }

        boolean hasPlugins = false;
        for (File file : files) {
            if (isPluginJar(file)) {
                if (!hasPlugins) {
                    pluginLoader.printLoadHeader();
                    hasPlugins = true;
                }
                loadPluginFile(file);
            }
        }
        if (hasPlugins) {
            pluginLoader.printLoadSummary();
        }
    }

    private boolean ensurePluginDirectory(File pluginsDir) {
        if (pluginsDir.exists()) {
            if (!pluginsDir.isDirectory()) {
                Axiom.logger().warn("plugins is not a directory");
                return false;
            }
            return true;
        }

        Axiom.logger().info("Plugins directory does not exist, creating it");
        if (!pluginsDir.mkdirs()) {
            Axiom.logger().warn("Failed to create plugins directory");
            return false;
        }
        return true;
    }

    // ────────────────────────────────────────────────────────
    // Plugin Loading Helpers
    // ────────────────────────────────────────────────────────

    private boolean isPluginJar(File file) {
        return file.isFile() && file.getName().endsWith(".jar");
    }

    private void loadPluginFile(File file) {
        try {
            pluginLoader.loadPlugin(file);
        } catch (Exception e) {
            LOGGER.error("Failed to load plugin from file: {}", file.getName(), e);
        }
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
        String host =
            minecraftServer.getLocalIp().isEmpty() ? "localhost" : minecraftServer.getLocalIp();
        ;
        int port = minecraftServer.getPort();
        return new Server(serverId, host, port);
    }

    public Collection<World> worlds() {
        return Collections.unmodifiableCollection(worlds.values());
    }

    public Optional<World> world(String name) {
        return Optional.ofNullable(worlds.get(name));
    }

    public GuiManager guiManager() {
        return guiManager;
    }

    public SidebarManager sidebarManager() {
        return sidebarManager;
    }

    public ScreenManager screenManager() {
        return screenManager;
    }

    public MinecraftServer minecraftServer() {
        return minecraftServer;
    }

    public static AxiomMod getInstance() {
        return instance;
    }
}
