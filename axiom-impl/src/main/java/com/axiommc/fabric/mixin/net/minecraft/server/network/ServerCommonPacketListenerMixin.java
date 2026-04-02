package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.ClientBrandTracker;
import com.axiommc.fabric.event.adapter.PlayerChannelAdapter;
import com.axiommc.fabric.event.adapter.PlayerTransferAdapter;
import com.axiommc.fabric.event.adapter.ResourcePackAdapter;
import net.minecraft.network.protocol.Packet;
import net.minecraft.network.protocol.common.ClientboundResourcePackPopPacket;
import net.minecraft.network.protocol.common.ClientboundResourcePackPushPacket;
import net.minecraft.network.protocol.common.ClientboundTransferPacket;
import net.minecraft.network.protocol.common.ServerboundCustomPayloadPacket;
import net.minecraft.network.protocol.common.ServerboundResourcePackPacket;
import net.minecraft.network.protocol.common.custom.BrandPayload;
import net.minecraft.network.protocol.common.custom.CustomPacketPayload;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.server.network.ServerCommonPacketListenerImpl;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Unique;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts custom payload, resource pack response, and outgoing
 * resource pack push/pop packets.
 */
@Mixin(value = ServerCommonPacketListenerImpl.class, remap = false)
public abstract class ServerCommonPacketListenerMixin {

    @Inject(method = "handleCustomPayload", at = @At("HEAD"))
    private void onCustomPayload(ServerboundCustomPayloadPacket packet, CallbackInfo callbackInfo) {
        CustomPacketPayload payload = packet.payload();
        ServerCommonPacketListenerImpl self = (ServerCommonPacketListenerImpl) (Object) this;

        if (payload instanceof BrandPayload(String brand)) {
            ServerPlayer player = getPlayerFromListener(self);
            if (player != null) {
                PlayerChannelAdapter.onClientBrand(player, brand);
            } else {
                ClientBrandTracker.store(self, brand);
            }
        }
    }

    @Inject(method = "handleResourcePackResponse", at = @At("HEAD"))
    private void onResourcePackResponse(
        ServerboundResourcePackPacket packet, CallbackInfo callbackInfo) {
        ServerCommonPacketListenerImpl self = (ServerCommonPacketListenerImpl) (Object) this;
        ServerPlayer player = getPlayerFromListener(self);
        if (player != null) {
            ResourcePackAdapter.onResourcePackStatus(player, packet.action());
        }
    }

    @Inject(
        method = "send(Lnet/minecraft/network/protocol/Packet;)V",
        at = @At("HEAD"),
        cancellable = true)
    private void onSendPacket(Packet<?> packet, CallbackInfo callbackInfo) {
        ServerCommonPacketListenerImpl self = (ServerCommonPacketListenerImpl) (Object) this;
        ServerPlayer player = getPlayerFromListener(self);
        if (player == null) {
            return;
        }

        if (packet instanceof ClientboundResourcePackPushPacket pushPacket) {
            ResourcePackAdapter.onResourcePackSend(
                player, pushPacket.url(), pushPacket.hash(), pushPacket.required());
        } else if (packet instanceof ClientboundResourcePackPopPacket popPacket) {
            ResourcePackAdapter.onResourcePackRemove(player);
        } else if (packet instanceof ClientboundTransferPacket transferPacket) {
            if (PlayerTransferAdapter.onPreTransfer(
                player, transferPacket.host(), transferPacket.port())) {
                callbackInfo.cancel();
                return;
            }
            PlayerTransferAdapter.onPostTransfer(
                player, transferPacket.host(), transferPacket.port());
        }
    }

    @Unique
    private static ServerPlayer getPlayerFromListener(ServerCommonPacketListenerImpl listener) {
        if (listener
            instanceof net.minecraft.server.network.ServerGamePacketListenerImpl gameListener) {
            return gameListener.getPlayer();
        }
        return null;
    }
}
