package com.axiommc.fabric.plugin;

import com.axiommc.api.bossbar.BossBar;
import com.axiommc.api.command.Command;
import com.axiommc.api.config.PluginConfig;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.PlayerManager;
import com.axiommc.api.plugin.PluginContext;
import com.axiommc.api.service.ServiceRegistry;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.BossBarManager;
import com.axiommc.fabric.AxiomMod;
import com.axiommc.fabric.config.TomlPluginConfig;
import com.axiommc.fabric.gui.FabricGuiManager;
import com.axiommc.fabric.player.FabricPlayerProvider;
import com.axiommc.fabric.sidebar.FabricSidebarManager;
import net.minecraft.server.MinecraftServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

public class SimplePluginContext implements PluginContext {

    private final String pluginId;
    private final String pluginName;
    private final EventBus eventBus;
    private final PlayerManager playerManager;
    private final BossBarManager bossBarManager;
    private final PluginConfig pluginConfig;
    private final File dataFolder;
    private final Logger logger;
    private final GuiManager guiManager;
    private final ServiceRegistry serviceRegistry;
    private SidebarManager sidebarManager;

    public SimplePluginContext(String pluginId, String pluginName, EventBus eventBus, FabricPlayerProvider playerProvider) {
        this.pluginId = pluginId;
        this.pluginName = pluginName;
        this.eventBus = eventBus;
        this.playerManager = playerProvider;
        this.bossBarManager = new InlineBossBarManager();
        this.dataFolder = new File("plugins/" + pluginName);
        this.pluginConfig = new TomlPluginConfig(new File(dataFolder, "config.toml"));
        this.logger = LoggerFactory.getLogger(pluginName);
        this.guiManager = new FabricGuiManager();
        this.serviceRegistry = new SimpleServiceRegistry();
        this.sidebarManager = null; // Initialized lazily in sidebarManager() method
    }

    private static class InlineBossBarManager implements BossBarManager {
        private final Map<String, BossBar> bossBars = new HashMap<>();

        @Override
        public BossBar create(BossBar.Spec spec) {
            String id = UUID.randomUUID().toString();
            BossBar bossBar = new SimpleBossBar(id);
            bossBars.put(id, bossBar);
            return bossBar;
        }

        @Override
        public BossBar get(String id) {
            return bossBars.get(id);
        }
    }

    private static class SimpleBossBar implements BossBar {
        private final String id;
        private boolean active = true;

        SimpleBossBar(String id) {
            this.id = id;
        }

        @Override
        public void title(com.axiommc.api.chat.ChatComponent title) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void process(float progress) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void color(com.axiommc.api.bossbar.BossBarColor color) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void style(com.axiommc.api.bossbar.BossBarStyle style) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void addPlayer(com.axiommc.api.player.Player player) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void removePlayer(com.axiommc.api.player.Player player) {
            // Placeholder - would integrate with actual Minecraft boss bar
        }

        @Override
        public void destroy() {
            active = false;
        }

        @Override
        public boolean isActive() {
            return active;
        }
    }

    @Override
    public EventBus eventBus() {
        return eventBus;
    }

    @Override
    public ServiceRegistry services() {
        return serviceRegistry;
    }

    @Override
    public PlayerManager players() {
        return playerManager;
    }

    @Override
    public BossBarManager bossBar() {
        return bossBarManager;
    }

    @Override
    public GuiManager guiManager() {
        return guiManager;
    }

    @Override
    public SidebarManager sidebarManager() {
        if (sidebarManager == null) {
            MinecraftServer server = AxiomMod.instance().minecraftServer();
            if (server == null) {
                throw new IllegalStateException("MinecraftServer not available yet");
            }
            sidebarManager = new FabricSidebarManager(server);
        }
        return sidebarManager;
    }

    @Override
    public void registerCommand(Command command) {
        AxiomMod mod = AxiomMod.instance();
        if (mod != null && mod.commandHandler() != null) {
            mod.commandHandler().registerCommand(command);
        }
    }

    @Override
    public PluginConfig config() {
        return pluginConfig;
    }

    @Override
    public File dataFolder() {
        // Ensure it exists
        if (!dataFolder.exists()) {
            dataFolder.mkdirs();
        }
        return dataFolder;
    }

    @Override
    public Logger logger() {
        return logger;
    }
}
