package com.axiommc.api.particle;

/**
 * Common particle effects available in the game.
 *
 * <p>Particles are categorized into two types: those with no data (simple
 * effects) and those with data (requiring additional parameters). Use
 * {@link #builder()} to create particle effects with custom settings.
 */
public enum Particle implements ParticleType {

    // ── No-data particles ─────────────────────────────────────────────────────

    /**
     * Flame particle effect.
     */
    FLAME("minecraft:flame", null),

    /**
     * Smoke particle effect.
     */
    SMOKE("minecraft:smoke", null),

    /**
     * Large smoke particle effect.
     */
    LARGE_SMOKE("minecraft:large_smoke", null),

    /**
     * Explosion particle effect.
     */
    EXPLOSION("minecraft:explosion", null),

    /**
     * Explosion emitter particle effect.
     */
    EXPLOSION_EMITTER("minecraft:explosion_emitter", null),

    /**
     * Critical hit particle effect.
     */
    CRIT("minecraft:crit", null),

    /**
     * Enchanted hit particle effect.
     */
    ENCHANTED_HIT("minecraft:enchanted_hit", null),

    /**
     * Happy villager particle effect.
     */
    HAPPY_VILLAGER("minecraft:happy_villager", null),

    /**
     * Angry villager particle effect.
     */
    ANGRY_VILLAGER("minecraft:angry_villager", null),

    /**
     * Heart particle effect.
     */
    HEART("minecraft:heart", null),

    /**
     * Portal particle effect.
     */
    PORTAL("minecraft:portal", null),

    /**
     * End rod particle effect.
     */
    END_ROD("minecraft:end_rod", null),

    /**
     * Witch particle effect.
     */
    WITCH("minecraft:witch", null),

    /**
     * Dripping water particle effect.
     */
    DRIPPING_WATER("minecraft:dripping_water", null),

    /**
     * Dripping lava particle effect.
     */
    DRIPPING_LAVA("minecraft:dripping_lava", null),

    /**
     * Snowflake particle effect.
     */
    SNOWFLAKE("minecraft:snowflake", null),

    /**
     * Totem of undying particle effect.
     */
    TOTEM_OF_UNDYING("minecraft:totem_of_undying", null),

    /**
     * Note particle effect.
     */
    NOTE("minecraft:note", null),

    /**
     * Bubble particle effect.
     */
    BUBBLE("minecraft:bubble", null),

    /**
     * Splash particle effect.
     */
    SPLASH("minecraft:splash", null),

    // ── Data particles ────────────────────────────────────────────────────────

    /**
     * Dust particle effect (requires DustData).
     */
    DUST("minecraft:dust", DustData.class),

    /**
     * Dust color transition particle effect (requires DustTransitionData).
     */
    DUST_COLOR_TRANSITION("minecraft:dust_color_transition", DustTransitionData.class),

    /**
     * Block particle effect (requires BlockData).
     */
    BLOCK("minecraft:block", BlockData.class),

    /**
     * Block marker particle effect (requires BlockData).
     */
    BLOCK_MARKER("minecraft:block_marker", BlockData.class),

    /**
     * Falling dust particle effect (requires BlockData).
     */
    FALLING_DUST("minecraft:falling_dust", BlockData.class),

    /**
     * Item particle effect (requires ItemData).
     */
    ITEM("minecraft:item", ItemData.class),

    /**
     * Entity effect particle effect (requires ColorData).
     */
    ENTITY_EFFECT("minecraft:entity_effect", ColorData.class);

    private final String key;
    private final Class<?> dataType;

    Particle(String key, Class<?> dataType) {
        this.key = key;
        this.dataType = dataType;
    }

    @Override
    public String key() {
        return key;
    }

    @Override
    public Class<?> dataType() {
        return dataType;
    }

    /**
     * Creates a builder pre-configured with this particle type.
     *
     * @return a new particle effect builder
     */
    public ParticleEffect.Builder builder() {
        return new ParticleEffect.Builder(this);
    }
}
