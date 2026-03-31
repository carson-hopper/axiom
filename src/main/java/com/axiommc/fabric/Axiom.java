package com.axiommc.fabric;

import com.axiommc.api.gui.GuiManager;
import com.axiommc.api.player.Player;
import com.axiommc.api.sidebar.SidebarManager;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.util.TaskScheduler;
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

    public static Collection<? extends Player> players() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.playerProvider().onlinePlayers();
    }

    public static Optional<? extends Player> player(UUID uuid) {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        return mod.playerProvider().player(uuid);
    }

    public static Optional<? extends Player> player(String name) {
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
        var server = mod.minecraftServer();
        if (server == null) {
            throw new IllegalStateException("Server not started yet");
        }
        // Return a singleton GuiManager via AxiomMod
        // For now, create instances per call since GuiManager is stateless
        return new com.axiommc.fabric.gui.FabricGuiManager();
    }

    public static SidebarManager sidebarManager() {
        AxiomMod mod = AxiomMod.instance();
        if (mod == null) {
            throw new IllegalStateException("Axiom is not initialized");
        }
        var server = mod.minecraftServer();
        if (server == null) {
            throw new IllegalStateException("Server not started yet");
        }
        return new com.axiommc.fabric.sidebar.FabricSidebarManager(server);
    }
}
