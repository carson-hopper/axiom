package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.PlayerSettingsAdapter;
import net.minecraft.network.protocol.common.ServerboundClientInformationPacket;
import net.minecraft.server.level.ClientInformation;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.server.network.ServerGamePacketListenerImpl;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts client information packets to fire PlayerSettingsChangedEvent.
 */
@Mixin(value = ServerGamePacketListenerImpl.class, remap = false)
public abstract class ServerGamePacketListenerMixin {

    @Shadow
    public abstract ServerPlayer getPlayer();

    @Inject(method = "handleClientInformation", at = @At("HEAD"))
    private void onClientInformation(
            ServerboundClientInformationPacket packet,
            CallbackInfo callbackInfo) {
        ClientInformation information = packet.information();
        PlayerSettingsAdapter.onSettingsChanged(
                getPlayer(),
                information.language(),
                information.viewDistance(),
                (information.modelCustomisation() & 0x01) != 0);
    }
}
