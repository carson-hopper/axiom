package com.axiommc.fabric.screen;

import com.axiommc.fabric.Axiom;
import net.minecraft.network.protocol.game.ClientboundPlayerAbilitiesPacket;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.level.GameType;

import java.lang.reflect.Field;

/**
 * Handles locking and unlocking the player's camera for screen sessions.
 *
 * <p>When locked:
 * <ul>
 *   <li>Player is set to ADVENTURE mode (prevents block interaction)</li>
 *   <li>Flying is disabled</li>
 *   <li>Walking speed set to 0 (prevents movement)</li>
 *   <li>Player is made invulnerable</li>
 * </ul>
 *
 * <p>The player's original state is stored in the ScreenSession for restoration.
 */
public final class ScreenCameraLock {

    private ScreenCameraLock() {}

    /**
     * Locks the player's movement and stores original state.
     *
     * @return the player's original game mode for later restoration
     */
    public static GameType lock(ServerPlayer player) {
        GameType originalMode = player.gameMode.getGameModeForPlayer();

        // Switch to adventure mode to prevent block interaction
        player.setGameMode(GameType.ADVENTURE);

        // Freeze movement by setting walk speed to 0
        setWalkSpeed(player, 0f);
        player.getAbilities().flying = false;
        player.getAbilities().mayfly = false;
        player.connection.send(new ClientboundPlayerAbilitiesPacket(player.getAbilities()));

        // Make invulnerable
        player.setInvulnerable(true);

        return originalMode;
    }

    /**
     * Restores the player's original state.
     */
    public static void unlock(ServerPlayer player, GameType originalMode) {
        // Restore game mode
        player.setGameMode(originalMode);

        // Restore movement speed
        setWalkSpeed(player, 0.1f); // default walk speed
        player.getAbilities().flying = originalMode == GameType.CREATIVE || originalMode == GameType.SPECTATOR;
        player.getAbilities().mayfly = originalMode == GameType.CREATIVE || originalMode == GameType.SPECTATOR;
        player.connection.send(new ClientboundPlayerAbilitiesPacket(player.getAbilities()));

        // Remove invulnerability (unless they were in creative)
        player.setInvulnerable(originalMode == GameType.CREATIVE);
    }

    private static void setWalkSpeed(ServerPlayer player, float speed) {
        try {
            Field field = player.getAbilities().getClass().getDeclaredField("walkingSpeed");
            field.setAccessible(true);
            field.setFloat(player.getAbilities(), speed);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set walk speed", e);
        }
    }
}
