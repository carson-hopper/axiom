package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.PlayerChannelAdapter;
import com.axiommc.fabric.event.adapter.ResourcePackAdapter;
import net.minecraft.network.protocol.Packet;
import net.minecraft.network.protocol.common.ClientboundResourcePackPopPacket;
import net.minecraft.network.protocol.common.ClientboundResourcePackPushPacket;
import net.minecraft.network.protocol.common.ServerboundCustomPayloadPacket;
import net.minecraft.network.protocol.common.ServerboundResourcePackPacket;
import net.minecraft.network.protocol.common.custom.BrandPayload;
import net.minecraft.network.protocol.common.custom.CustomPacketPayload;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.server.network.ServerCommonPacketListenerImpl;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts custom payload, resource pack response, and outgoing
 * resource pack push/pop packets. Fires PlayerClientBrandEvent,
 * PlayerChannelRegister/UnregisterEvent, PlayerResourcePackStatusEvent,
 * ServerResourcePackSendEvent, ServerResourcePackRequestEvent,
 * and ServerResourcePackRemoveEvent.
 */
@Mixin(value = ServerCommonPacketListenerImpl.class, remap = false)
public abstract class ServerCommonPacketListenerMixin {

    @Inject(method = "handleCustomPayload", at = @At("HEAD"))
    private void onCustomPayload(
            ServerboundCustomPayloadPacket packet,
            CallbackInfo callbackInfo) {
        CustomPacketPayload payload = packet.payload();
        ServerCommonPacketListenerImpl self =
                (ServerCommonPacketListenerImpl) (Object) this;

        if (payload instanceof BrandPayload brandPayload) {
            ServerPlayer player = getPlayerFromListener(self);
            if (player != null) {
                PlayerChannelAdapter.onClientBrand(
                        player, brandPayload.brand());
            }
        }
    }

    @Inject(method = "handleResourcePackResponse", at = @At("HEAD"))
    private void onResourcePackResponse(
            ServerboundResourcePackPacket packet,
            CallbackInfo callbackInfo) {
        ServerCommonPacketListenerImpl self =
                (ServerCommonPacketListenerImpl) (Object) this;
        ServerPlayer player = getPlayerFromListener(self);
        if (player != null) {
            ResourcePackAdapter.onResourcePackStatus(
                    player, packet.action());
        }
    }

    @Inject(method = "send(Lnet/minecraft/network/protocol/Packet;)V",
            at = @At("HEAD"))
    private void onSendPacket(Packet<?> packet, CallbackInfo callbackInfo) {
        ServerCommonPacketListenerImpl self =
                (ServerCommonPacketListenerImpl) (Object) this;
        ServerPlayer player = getPlayerFromListener(self);
        if (player == null) {
            return;
        }

        if (packet instanceof ClientboundResourcePackPushPacket pushPacket) {
            ResourcePackAdapter.onResourcePackSend(
                    player, pushPacket.url(), pushPacket.hash(),
                    pushPacket.required());
        } else if (packet instanceof ClientboundResourcePackPopPacket popPacket) {
            ResourcePackAdapter.onResourcePackRemove(player);
        }
    }

    /**
     * Extracts the ServerPlayer from the listener via reflection or casting.
     * ServerGamePacketListenerImpl has getPlayer(), but the common
     * superclass does not — try casting first.
     */
    private static ServerPlayer getPlayerFromListener(
            ServerCommonPacketListenerImpl listener) {
        try {
            if (listener instanceof net.minecraft.server.network.ServerGamePacketListenerImpl gameListener) {
                return gameListener.getPlayer();
            }
        } catch (Exception _) {
            // Not a game listener
        }
        return null;
    }
}
