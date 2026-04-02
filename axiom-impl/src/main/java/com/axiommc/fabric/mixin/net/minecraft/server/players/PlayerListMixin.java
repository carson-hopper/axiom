package com.axiommc.fabric.mixin.net.minecraft.server.players;

import com.axiommc.fabric.event.adapter.EntityEventAdapter;
import com.axiommc.fabric.event.adapter.PlayerLifecycleAdapter;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.server.players.PlayerList;
import net.minecraft.world.entity.Entity;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts player removal, respawn, and broadcast messages.
 */
@Mixin(value = PlayerList.class, remap = false)
public abstract class PlayerListMixin {

    @Inject(method = "remove", at = @At("HEAD"))
    private void onRemove(ServerPlayer serverPlayer, CallbackInfo callbackInfo) {
        PlayerLifecycleAdapter.onKick(serverPlayer, "Disconnected");
    }

    @Inject(method = "respawn", at = @At("HEAD"))
    private void onRespawn(
        ServerPlayer serverPlayer,
        boolean keepEverything,
        Entity.RemovalReason removalReason,
        CallbackInfoReturnable<ServerPlayer> callbackInfo) {
        PlayerLifecycleAdapter.onRespawn(serverPlayer);
    }

    @Inject(
        method = "broadcastSystemMessage(Lnet/minecraft/network/chat/Component;Z)V",
        at = @At("HEAD"),
        cancellable = true)
    private void onBroadcastSystemMessage(
        Component message, boolean overlay, CallbackInfo callbackInfo) {
        if (!EntityEventAdapter.onBroadcastMessage(message.getString())) {
            callbackInfo.cancel();
        }
    }
}
