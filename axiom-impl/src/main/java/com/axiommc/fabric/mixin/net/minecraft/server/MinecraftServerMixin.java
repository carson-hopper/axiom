package com.axiommc.fabric.mixin.net.minecraft.server;

import net.minecraft.server.MinecraftServer;
import org.spongepowered.asm.mixin.Mixin;

/**
 * Mixin on MinecraftServer for server-level event hooks.
 *
 * <p>Note: broadcastSystemMessage was moved to PlayerListMixin since the method
 * lives on PlayerList, not MinecraftServer, in 26.1.
 */
@Mixin(value = MinecraftServer.class, remap = false)
public abstract class MinecraftServerMixin {}
