package com.axiommc.fabric.sidebar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.sidebar.Sidebar;
import com.axiommc.api.sidebar.SidebarManager;
import net.minecraft.server.MinecraftServer;

public record FabricSidebarManager(MinecraftServer server) implements SidebarManager {

    @Override
    public Sidebar create(ChatComponent title) {
        return new FabricSidebar(title, server);
    }
}
