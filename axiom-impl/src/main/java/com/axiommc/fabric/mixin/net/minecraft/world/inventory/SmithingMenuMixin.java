package com.axiommc.fabric.mixin.net.minecraft.world.inventory;

import com.axiommc.fabric.event.adapter.InventoryEventAdapter;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.SmithingMenu;
import net.minecraft.world.item.ItemStack;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Fires {@link com.axiommc.api.event.inventory.InventoryEvent.Smith} when a
 * player takes the result from a smithing table.
 */
@Mixin(value = SmithingMenu.class, remap = false)
public abstract class SmithingMenuMixin {

    @Inject(method = "onTake", at = @At("HEAD"))
    private void onTake(Player player, ItemStack itemStack, CallbackInfo callbackInfo) {
        if (player instanceof ServerPlayer serverPlayer) {
            InventoryEventAdapter.onSmith(serverPlayer, itemStack);
        }
    }
}
