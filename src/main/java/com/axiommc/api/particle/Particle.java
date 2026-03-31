package com.axiommc.api.particle;

public enum Particle implements ParticleType {

    // ── No-data particles ─────────────────────────────────────────────────────
    FLAME                ("minecraft:flame",                    null),
    SMOKE                ("minecraft:smoke",                    null),
    LARGE_SMOKE          ("minecraft:large_smoke",              null),
    EXPLOSION            ("minecraft:explosion",                null),
    EXPLOSION_EMITTER    ("minecraft:explosion_emitter",        null),
    CRIT                 ("minecraft:crit",                     null),
    ENCHANTED_HIT        ("minecraft:enchanted_hit",            null),
    HAPPY_VILLAGER       ("minecraft:happy_villager",           null),
    ANGRY_VILLAGER       ("minecraft:angry_villager",           null),
    HEART                ("minecraft:heart",                    null),
    PORTAL               ("minecraft:portal",                   null),
    END_ROD              ("minecraft:end_rod",                  null),
    WITCH                ("minecraft:witch",                    null),
    DRIPPING_WATER       ("minecraft:dripping_water",           null),
    DRIPPING_LAVA        ("minecraft:dripping_lava",            null),
    SNOWFLAKE            ("minecraft:snowflake",                null),
    TOTEM_OF_UNDYING     ("minecraft:totem_of_undying",         null),
    NOTE                 ("minecraft:note",                     null),
    BUBBLE               ("minecraft:bubble",                   null),
    SPLASH               ("minecraft:splash",                   null),

    // ── Data particles ────────────────────────────────────────────────────────
    DUST                 ("minecraft:dust",                     DustData.class),
    DUST_COLOR_TRANSITION("minecraft:dust_color_transition",    DustTransitionData.class),
    BLOCK                ("minecraft:block",                    BlockData.class),
    BLOCK_MARKER         ("minecraft:block_marker",             BlockData.class),
    FALLING_DUST         ("minecraft:falling_dust",             BlockData.class),
    ITEM                 ("minecraft:item",                     ItemData.class),
    ENTITY_EFFECT        ("minecraft:entity_effect",            ColorData.class);

    private final String key;
    private final Class<?> dataType;

    Particle(String key, Class<?> dataType) {
        this.key      = key;
        this.dataType = dataType;
    }

    @Override public String key()        { return key; }
    @Override public Class<?> dataType() { return dataType; }

    /** Returns a builder pre-configured with this particle type. */
    public ParticleEffect.Builder builder() {
        return new ParticleEffect.Builder(this);
    }
}
