package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.ServerEventAdapter;
import net.minecraft.network.Connection;
import net.minecraft.network.protocol.status.ServerStatus;
import net.minecraft.network.protocol.status.ServerboundStatusRequestPacket;
import net.minecraft.server.network.ServerStatusPacketListenerImpl;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts status request handling to fire ServerListPingEvent.
 */
@Mixin(value = ServerStatusPacketListenerImpl.class, remap = false)
public abstract class ServerStatusPacketListenerMixin {

    @Shadow
    @Final
    private ServerStatus status;

    @Shadow
    @Final
    private Connection connection;

    @Inject(
        method = "handleStatusRequest",
        at =
            @At(
                value = "INVOKE",
                target =
                    "Lnet/minecraft/network/Connection;send(Lnet/minecraft/network/protocol/Packet;)V"))
    private void onStatusRequest(ServerboundStatusRequestPacket packet, CallbackInfo callbackInfo) {
        String address = connection.getRemoteAddress() != null
            ? connection.getRemoteAddress().toString()
            : "unknown";
        String motd = status.description().getString();
        int maxPlayers = 0;
        int onlinePlayers = 0;
        if (status.players().isPresent()) {
            ServerStatus.Players players = status.players().get();
            maxPlayers = players.max();
            onlinePlayers = players.online();
        }
        ServerEventAdapter.onServerListPing(address, motd, maxPlayers, onlinePlayers);
    }
}
