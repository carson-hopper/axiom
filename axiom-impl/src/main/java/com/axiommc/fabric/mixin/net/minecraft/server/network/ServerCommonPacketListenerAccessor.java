package com.axiommc.fabric.mixin.net.minecraft.server.network;

import net.minecraft.network.Connection;
import net.minecraft.server.network.ServerCommonPacketListenerImpl;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.gen.Accessor;

@Mixin(value = ServerCommonPacketListenerImpl.class, remap = false)
public interface ServerCommonPacketListenerAccessor {

    @Accessor("connection")
    Connection connection();
}
