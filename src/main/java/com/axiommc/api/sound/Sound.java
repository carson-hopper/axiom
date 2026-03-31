package com.axiommc.api.sound;

/**
 * Common sound effects available for playback.
 *
 * <p>Each sound is mapped to a Minecraft sound key and implements
 * the {@link SoundKey} interface.
 */
public enum Sound implements SoundKey {
    /**
     * Player level up sound.
     */
    LEVEL_UP("minecraft:entity.player.levelup"),

    /**
     * Experience orb pickup sound.
     */
    ORB_PICKUP("minecraft:entity.experience_orb.pickup"),

    /**
     * Button click sound.
     */
    CLICK("minecraft:ui.button.click"),

    /**
     * Chest opening sound.
     */
    CHEST_OPEN("minecraft:block.chest.open"),

    /**
     * Chest closing sound.
     */
    CHEST_CLOSE("minecraft:block.chest.close"),

    /**
     * Villager agreement sound.
     */
    VILLAGER_YES("minecraft:entity.villager.yes"),

    /**
     * Villager disagreement sound.
     */
    VILLAGER_NO("minecraft:entity.villager.no"),

    /**
     * Anvil usage sound.
     */
    ANVIL_USE("minecraft:block.anvil.use"),

    /**
     * Item pickup sound.
     */
    ITEM_PICKUP("minecraft:entity.item.pickup"),

    /**
     * Firework explosion sound.
     */
    FIREWORK("minecraft:entity.firework_rocket.blast"),

    /**
     * Player death sound.
     */
    PLAYER_DEATH("minecraft:entity.player.death");

    private final String key;

    Sound(String key) {
        this.key = key;
    }

    @Override
    public String key() {
        return key;
    }
}
