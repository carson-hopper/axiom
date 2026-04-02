package com.axiommc.fabric.mixin.net.minecraft.server.network;

import com.axiommc.fabric.event.adapter.PlayerInputAdapter;
import com.axiommc.fabric.event.adapter.PlayerInventoryAdapter;
import com.axiommc.fabric.event.adapter.PlayerMiscAdapter;
import com.axiommc.fabric.event.adapter.PlayerSettingsAdapter;
import net.minecraft.network.protocol.common.ServerboundClientInformationPacket;
import net.minecraft.network.protocol.game.ServerboundChatCommandPacket;
import net.minecraft.network.protocol.game.ServerboundContainerClickPacket;
import net.minecraft.network.protocol.game.ServerboundContainerClosePacket;
import net.minecraft.network.protocol.game.ServerboundEditBookPacket;
import net.minecraft.network.protocol.game.ServerboundInteractPacket;
import net.minecraft.network.protocol.game.ServerboundMovePlayerPacket;
import net.minecraft.network.protocol.game.ServerboundPlayerAbilitiesPacket;
import net.minecraft.network.protocol.game.ServerboundPlayerActionPacket;
import net.minecraft.network.protocol.game.ServerboundPlayerCommandPacket;
import net.minecraft.network.protocol.game.ServerboundPlayerInputPacket;
import net.minecraft.network.protocol.game.ServerboundSetCarriedItemPacket;
import net.minecraft.network.protocol.game.ServerboundSetCreativeModeSlotPacket;
import net.minecraft.network.protocol.game.ServerboundSwingPacket;
import net.minecraft.server.level.ClientInformation;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.server.network.ServerGamePacketListenerImpl;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.entity.player.Input;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

/**
 * Intercepts game packets to fire player-related events.
 */
@Mixin(value = ServerGamePacketListenerImpl.class, remap = false)
public abstract class ServerGamePacketListenerMixin {

    @Shadow
    public abstract ServerPlayer getPlayer();

    @Inject(method = "handleClientInformation", at = @At("HEAD"))
    private void onClientInformation(
        ServerboundClientInformationPacket packet, CallbackInfo callbackInfo) {
        ClientInformation information = packet.information();
        PlayerSettingsAdapter.onSettingsChanged(
            getPlayer(),
            information.language(),
            information.viewDistance(),
            (information.modelCustomisation() & 0x01) != 0);
    }

    @Inject(method = "handleAnimate", at = @At("HEAD"))
    private void onAnimate(ServerboundSwingPacket packet, CallbackInfo callbackInfo) {
        PlayerMiscAdapter.onAnimation(getPlayer(), "ARM_SWING");
    }

    @Inject(method = "handleEditBook", at = @At("HEAD"), cancellable = true)
    private void onEditBook(ServerboundEditBookPacket packet, CallbackInfo callbackInfo) {
        net.minecraft.world.item.ItemStack book = getPlayer().getInventory().getItem(packet.slot());
        if (PlayerMiscAdapter.onEditBook(getPlayer(), book)) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handlePlayerAbilities", at = @At("HEAD"), cancellable = true)
    private void onPlayerAbilities(
        ServerboundPlayerAbilitiesPacket packet, CallbackInfo callbackInfo) {
        if (PlayerMiscAdapter.onToggleFlight(getPlayer(), packet.isFlying())) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handleMovePlayer", at = @At("HEAD"), cancellable = true)
    private void onMovePlayer(ServerboundMovePlayerPacket packet, CallbackInfo callbackInfo) {
        ServerPlayer player = getPlayer();
        double fromX = player.getX();
        double fromY = player.getY();
        double fromZ = player.getZ();
        double toX = packet.getX(fromX);
        double toY = packet.getY(fromY);
        double toZ = packet.getZ(fromZ);
        if (PlayerInputAdapter.onMove(player, fromX, fromY, fromZ, toX, toY, toZ)) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handleChatCommand", at = @At("HEAD"), cancellable = true)
    private void onChatCommand(ServerboundChatCommandPacket packet, CallbackInfo callbackInfo) {
        if (PlayerInputAdapter.onCommand(getPlayer(), packet.command())) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handleInteract", at = @At("HEAD"), cancellable = true)
    private void onInteract(ServerboundInteractPacket packet, CallbackInfo callbackInfo) {
        boolean mainHand = packet.hand() == InteractionHand.MAIN_HAND;
        if (PlayerInputAdapter.onInteractEntity(getPlayer(), packet.entityId(), mainHand)) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handlePlayerCommand", at = @At("HEAD"))
    private void onPlayerCommand(ServerboundPlayerCommandPacket packet, CallbackInfo callbackInfo) {
        ServerboundPlayerCommandPacket.Action action = packet.getAction();
        if (action == ServerboundPlayerCommandPacket.Action.START_SPRINTING) {
            PlayerInputAdapter.onToggleSprint(getPlayer(), true);
        } else if (action == ServerboundPlayerCommandPacket.Action.STOP_SPRINTING) {
            PlayerInputAdapter.onToggleSprint(getPlayer(), false);
        }
    }

    @Inject(method = "handlePlayerInput", at = @At("HEAD"))
    private void onPlayerInput(ServerboundPlayerInputPacket packet, CallbackInfo callbackInfo) {
        Input input = packet.input();
        PlayerMiscAdapter.onInput(
            getPlayer(),
            input.forward(),
            input.backward(),
            input.left(),
            input.right(),
            input.jump(),
            input.shift(),
            input.sprint());
        boolean sneaking = input.shift();
        if (sneaking != getPlayer().isShiftKeyDown()) {
            PlayerInputAdapter.onToggleSneak(getPlayer(), sneaking);
        }
    }

    @Inject(method = "handlePlayerAction", at = @At("HEAD"), cancellable = true)
    private void onPlayerAction(ServerboundPlayerActionPacket packet, CallbackInfo callbackInfo) {
        if (packet.getAction() == ServerboundPlayerActionPacket.Action.SWAP_ITEM_WITH_OFFHAND) {
            if (PlayerInputAdapter.onSwapHandItems(getPlayer())) {
                callbackInfo.cancel();
            }
        }
    }

    @Inject(method = "handleContainerClick", at = @At("HEAD"), cancellable = true)
    private void onContainerClick(
        ServerboundContainerClickPacket packet, CallbackInfo callbackInfo) {
        net.minecraft.world.item.ItemStack carried = getPlayer().containerMenu.getCarried();
        if (PlayerInventoryAdapter.onClick(getPlayer(), packet.slotNum(), carried)) {
            callbackInfo.cancel();
        }
    }

    @Inject(method = "handleContainerClose", at = @At("HEAD"))
    private void onContainerClose(
        ServerboundContainerClosePacket packet, CallbackInfo callbackInfo) {
        PlayerInventoryAdapter.onClose(getPlayer());
    }

    @Inject(method = "handleSetCarriedItem", at = @At("HEAD"))
    private void onSetCarriedItem(
        ServerboundSetCarriedItemPacket packet, CallbackInfo callbackInfo) {
        PlayerInventoryAdapter.onHeldItemChange(getPlayer(), packet.getSlot());
    }

    @Inject(method = "handleSetCreativeModeSlot", at = @At("HEAD"), cancellable = true)
    private void onSetCreativeModeSlot(
        ServerboundSetCreativeModeSlotPacket packet, CallbackInfo callbackInfo) {
        if (PlayerInventoryAdapter.onCreativeSlot(
            getPlayer(), packet.slotNum(), packet.itemStack())) {
            callbackInfo.cancel();
        }
    }
}
