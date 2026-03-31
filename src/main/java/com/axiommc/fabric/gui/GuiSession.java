package com.axiommc.fabric.gui;

import com.axiommc.api.gui.Gui;
import net.minecraft.server.level.ServerPlayer;
import java.util.UUID;

public record GuiSession(UUID sessionId, ServerPlayer player, AxiomChestMenu menu, Gui gui) {}
