package com.axiommc.fabric;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.Player;
import com.axiommc.api.screen.ScreenManager;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.console.AxiomLogger;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.util.TaskScheduler;

import java.util.Collection;
import java.util.UUID;
import java.util.Optional;

public class Axiom {

    private static final AxiomLogger LOGGER = new AxiomLogger("axiom");

    private Axiom() {}

    // ============================================================
    // Static Accessors
    // ============================================================

    public static AxiomLogger logger() {
        return LOGGER;
    }

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

    public static ScreenManager screenManager() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        ScreenManager manager = mod.screenManager();
        if (manager == null) {
            throw new IllegalStateException("Server not started yet");
        }
        return manager;
    }
}
