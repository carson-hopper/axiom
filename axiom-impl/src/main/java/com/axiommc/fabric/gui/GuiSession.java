package com.axiommc.fabric.gui;

import com.axiommc.api.gui.Gui;
import java.util.UUID;
import net.minecraft.server.level.ServerPlayer;

public record GuiSession(UUID sessionId, ServerPlayer player, AxiomChestMenu menu, Gui gui) {}
