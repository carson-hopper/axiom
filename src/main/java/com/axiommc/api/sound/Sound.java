package com.axiommc.api.sound;

public enum Sound implements SoundKey {
    LEVEL_UP("minecraft:entity.player.levelup"),
    ORB_PICKUP("minecraft:entity.experience_orb.pickup"),
    CLICK("minecraft:ui.button.click"),
    CHEST_OPEN("minecraft:block.chest.open"),
    CHEST_CLOSE("minecraft:block.chest.close"),
    VILLAGER_YES("minecraft:entity.villager.yes"),
    VILLAGER_NO("minecraft:entity.villager.no"),
    ANVIL_USE("minecraft:block.anvil.use"),
    ITEM_PICKUP("minecraft:entity.item.pickup"),
    FIREWORK("minecraft:entity.firework_rocket.blast"),
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
