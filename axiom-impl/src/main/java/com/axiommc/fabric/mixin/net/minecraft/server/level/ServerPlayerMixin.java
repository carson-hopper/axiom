package com.axiommc.fabric.mixin.net.minecraft.server.level;

import com.axiommc.fabric.event.adapter.PlayerActionAdapter;
import com.axiommc.fabric.event.adapter.PlayerInventoryAdapter;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.MenuProvider;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.item.ItemEntity;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.GameType;
import net.minecraft.world.level.portal.TeleportTransition;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts player actions on {@link ServerPlayer} to fire player events.
 */
@Mixin(value = ServerPlayer.class, remap = false)
public abstract class ServerPlayerMixin {

    @Inject(method = "die", at = @At("HEAD"))
    private void onDie(DamageSource damageSource, CallbackInfo callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        PlayerActionAdapter.onDeath(
            self, damageSource.getLocalizedDeathMessage(self).getString());
    }

    @Inject(method = "hurtServer", at = @At("HEAD"), cancellable = true)
    private void onHurtServer(
        ServerLevel serverLevel,
        DamageSource damageSource,
        float damage,
        CallbackInfoReturnable<Boolean> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        if (PlayerActionAdapter.onDamage(self, damage, damageSource.type().msgId())) {
            callbackInfo.setReturnValue(false);
        }
    }

    @Inject(
        method =
            "drop(Lnet/minecraft/world/item/ItemStack;ZZ)Lnet/minecraft/world/entity/item/ItemEntity;",
        at = @At("HEAD"),
        cancellable = true)
    private void onDrop(
        ItemStack itemStack,
        boolean throwRandomly,
        boolean retainOwnership,
        CallbackInfoReturnable<ItemEntity> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        if (PlayerActionAdapter.onDropItem(self, itemStack)) {
            callbackInfo.setReturnValue(null);
        }
    }

    @Inject(method = "onItemPickup", at = @At("HEAD"))
    private void onItemPickup(ItemEntity itemEntity, CallbackInfo callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        PlayerActionAdapter.onPickupItem(self, itemEntity.getItem());
    }

    @Inject(method = "setGameMode", at = @At("HEAD"), cancellable = true)
    private void onSetGameMode(GameType gameType, CallbackInfoReturnable<Boolean> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        if (PlayerActionAdapter.onGameModeChange(self, gameType.getName())) {
            callbackInfo.setReturnValue(false);
        }
    }

    @Inject(
        method =
            "teleport(Lnet/minecraft/world/level/portal/TeleportTransition;)Lnet/minecraft/server/level/ServerPlayer;",
        at = @At("HEAD"),
        cancellable = true)
    private void onTeleport(
        TeleportTransition transition, CallbackInfoReturnable<ServerPlayer> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        if (PlayerActionAdapter.onTeleport(self, transition)) {
            callbackInfo.setReturnValue(null);
        }
    }

    @Inject(method = "startRiding", at = @At("HEAD"), cancellable = true)
    private void onStartRiding(
        Entity entity,
        boolean force,
        boolean suppressEvent,
        CallbackInfoReturnable<Boolean> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        if (PlayerActionAdapter.onMount(self, entity)) {
            callbackInfo.setReturnValue(false);
        }
    }

    @Inject(method = "removeVehicle", at = @At("HEAD"))
    private void onRemoveVehicle(CallbackInfo callbackInfo) {
        Entity vehicle = ((Entity) (Object) this).getVehicle();
        if (vehicle != null) {
            ServerPlayer self = (ServerPlayer) (Object) this;
            PlayerActionAdapter.onDismount(self, vehicle);
        }
    }

    @Inject(method = "openMenu", at = @At("HEAD"))
    private void onOpenMenu(
        MenuProvider menuProvider, CallbackInfoReturnable<java.util.OptionalInt> callbackInfo) {
        ServerPlayer self = (ServerPlayer) (Object) this;
        PlayerInventoryAdapter.onOpen(self);
    }
}
