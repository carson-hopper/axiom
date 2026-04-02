package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.GameProfileAdapter;
import net.minecraft.network.protocol.login.ServerboundHelloPacket;
import net.minecraft.server.network.ServerLoginPacketListenerImpl;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts the login hello packet to fire GameProfileRequestEvent.
 */
@Mixin(value = ServerLoginPacketListenerImpl.class, remap = false)
public abstract class ServerLoginPacketListenerMixin {

    @Inject(method = "handleHello", at = @At("HEAD"))
    private void onHello(ServerboundHelloPacket packet, CallbackInfo callbackInfo) {
        GameProfileAdapter.onGameProfileRequest(packet.name(), packet.profileId());
    }
}
