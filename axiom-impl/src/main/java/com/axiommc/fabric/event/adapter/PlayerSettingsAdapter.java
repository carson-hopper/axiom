package com.axiommc.fabric.event.adapter;

import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.player.PlayerSettingsChangedEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.player.FabricPlayerProvider;
import net.minecraft.server.level.ServerPlayer;

/**
 * Fires {@link PlayerSettingsChangedEvent} when a player's client
 * settings change. Called from {@code ServerGamePacketListenerMixin}.
 */
public class PlayerSettingsAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;
    private static FabricPlayerProvider playerProvider;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        PlayerSettingsAdapter.eventBus = eventBus;
        PlayerSettingsAdapter.playerProvider = playerProvider;
    }

    /**
     * Called from the mixin when a client information packet arrives.
     *
     * @param serverPlayer the player whose settings changed
     * @param language     the client language code
     * @param viewDistance  the client view distance
     * @param showCape     whether the cape skin layer is enabled
     */
    public static void onSettingsChanged(ServerPlayer serverPlayer, String language, int viewDistance, boolean showCape) {
        if (eventBus == null) {
            return;
        }
        try {
            FabricPlayer player = new FabricPlayer(serverPlayer);
            eventBus.publish(new PlayerSettingsChangedEvent(
                    player, language, viewDistance, showCape));
        } catch (Exception exception) {
            Axiom.logger().debug(
                    "Error firing PlayerSettingsChangedEvent", exception);
        }
    }
}
