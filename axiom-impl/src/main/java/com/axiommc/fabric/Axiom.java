package com.axiommc.fabric;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.Player;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.chat.ConsoleColorFormatter;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.util.TaskScheduler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collection;
import java.util.UUID;
import java.util.Optional;

public class Axiom {

    private Axiom() {
        // Prevent instantiation
    }

    // ============================================================
    // Static Accessors
    // ============================================================

    public static TaskScheduler scheduler() {
        return TaskScheduler.global();
    }

    public static Collection<FabricPlayer> players() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.playerProvider().onlinePlayers();
    }

    public static Optional<FabricPlayer> player(UUID uuid) {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.playerProvider().player(uuid);
    }

    public static Optional<FabricPlayer> player(String name) {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.playerProvider().player(name);
    }

    public static Server server() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.server();
    }

    public static Collection<World> worlds() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.worlds();
    }

    public static Optional<World> world(String name) {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.world(name);
    }

    public static GuiManager guiManager() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        GuiManager manager = mod.guiManager();
        if (manager == null) {
            throw new IllegalStateException("Server not started yet");
        }
        return manager;
    }

    /**
     * Sends a formatted message to the server console with ANSI color support.
     */
    public static void consoleSendMessage(ChatComponent message) {
        Logger logger = LoggerFactory.getLogger("axiom");
        logger.info(ConsoleColorFormatter.format(message));
    }

    /**
     * Sends a plain string message to the server console.
     */
    public static void consoleSendMessage(String message) {
        Logger logger = LoggerFactory.getLogger("axiom");
        logger.info(ConsoleColorFormatter.formatWithAnsi(message));
    }

    public static SidebarManager sidebarManager() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        SidebarManager manager = mod.sidebarManager();
        if (manager == null) {
            throw new IllegalStateException("Server not started yet");
        }
        return manager;
    }
}
