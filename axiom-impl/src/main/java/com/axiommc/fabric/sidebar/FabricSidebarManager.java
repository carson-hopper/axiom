package com.axiommc.fabric.sidebar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.sidebar.Sidebar;
import com.axiommc.api.sidebar.SidebarManager;
import net.minecraft.server.MinecraftServer;

public class FabricSidebarManager implements SidebarManager {
    private final MinecraftServer server;

    public FabricSidebarManager(MinecraftServer server) {
        this.server = server;
    }

    @Override
    public Sidebar create(ChatComponent title) {
        return new FabricSidebar(title, server);
    }
}
