package com.axiommc.fabric.mixin.net.minecraft.world.inventory;

import com.axiommc.fabric.event.adapter.InventoryEventAdapter;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.Container;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.EnchantmentMenu;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

/**
 * Intercepts enchantment table interactions to fire enchantment events.
 */
@Mixin(value = EnchantmentMenu.class, remap = false)
public abstract class EnchantmentMenuMixin {

    @Shadow
    @Final
    private Container enchantSlots;

    @Inject(method = "clickMenuButton", at = @At("HEAD"), cancellable = true)
    private void onClickMenuButton(
        Player player, int buttonId, CallbackInfoReturnable<Boolean> callbackInfo) {
        if (player instanceof ServerPlayer serverPlayer) {
            net.minecraft.world.item.ItemStack item = enchantSlots.getItem(0);
            if (!item.isEmpty()
                && InventoryEventAdapter.onEnchant(serverPlayer, item, buttonId + 1)) {
                callbackInfo.setReturnValue(false);
            }
        }
    }
}
