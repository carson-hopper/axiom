package com.axiommc.api.entity.display;

import com.axiommc.api.math.Vector3;

public final class BlockDisplaySpec {

    // Shared base fields
    private final Billboard         billboard;
    private final float             scale;
    private final float             yaw;
    private final Vector3           translation;
    private final DisplayTransform  rawTransform;
    private final float             viewRange;
    private final float             shadowRadius;
    private final float             shadowStrength;
    private final DisplayBrightness brightness;
    private final Integer           glowColorOverride;
    private final int               ttl;

    // Block-specific fields
    private final String block; // e.g. "minecraft:stone"

    private BlockDisplaySpec(Builder b) {
        this.billboard         = b.billboard;
        this.scale             = b.scale;
        this.yaw               = b.yaw;
        this.translation       = b.translation;
        this.rawTransform      = b.rawTransform;
        this.viewRange         = b.viewRange;
        this.shadowRadius      = b.shadowRadius;
        this.shadowStrength    = b.shadowStrength;
        this.brightness        = b.brightness;
        this.glowColorOverride = b.glowColorOverride;
        this.ttl               = b.ttl;
        this.block             = b.block;
    }

    // Getters
    public Billboard         billboard()         { return billboard; }
    public float             scale()             { return scale; }
    public float             yaw()               { return yaw; }
    public Vector3           translation()       { return translation; }
    public DisplayTransform  rawTransform()      { return rawTransform; }
    public float             viewRange()         { return viewRange; }
    public float             shadowRadius()      { return shadowRadius; }
    public float             shadowStrength()    { return shadowStrength; }
    public DisplayBrightness brightness()        { return brightness; }
    public Integer           glowColorOverride() { return glowColorOverride; }
    public int               ttl()               { return ttl; }
    public String            block()             { return block; }

    public Builder toBuilder() { return new Builder().copyFrom(this); }
    public static Builder spec() { return new Builder(); }

    public static final class Builder {
        private Billboard         billboard        = Billboard.FIXED;
        private float             scale            = 1.0f;
        private float             yaw              = 0f;
        private Vector3           translation      = Vector3.ZERO;
        private DisplayTransform  rawTransform     = null;
        private float             viewRange        = 1.0f;
        private float             shadowRadius     = 0f;
        private float             shadowStrength   = 1.0f;
        private DisplayBrightness brightness       = null;
        private Integer           glowColorOverride = null;
        private int               ttl              = 0;
        private String            block            = null; // required

        public Builder billboard(Billboard b)            { billboard = b; return this; }
        public Builder scale(float s)                    { scale = s; return this; }
        public Builder yaw(float degrees)                { yaw = degrees; return this; }
        public Builder translation(Vector3 t)            { translation = t; return this; }
        public Builder rawTransform(DisplayTransform t)  { rawTransform = t; return this; }
        public Builder viewRange(float r)                { viewRange = r; return this; }
        public Builder shadowRadius(float r)             { shadowRadius = r; return this; }
        public Builder shadowStrength(float s)           { shadowStrength = s; return this; }
        public Builder brightness(DisplayBrightness b)   { brightness = b; return this; }
        public Builder glowColorOverride(int argb)       { glowColorOverride = argb; return this; }
        public Builder clearGlowColorOverride()          { glowColorOverride = null; return this; }
        public Builder ttl(int ticks)                    { ttl = ticks; return this; }
        public Builder block(String blockId)             { block = blockId; return this; }

        public Builder copyFrom(BlockDisplaySpec spec) {
            billboard         = spec.billboard;
            scale             = spec.scale;
            yaw               = spec.yaw;
            translation       = spec.translation;
            rawTransform      = spec.rawTransform;
            viewRange         = spec.viewRange;
            shadowRadius      = spec.shadowRadius;
            shadowStrength    = spec.shadowStrength;
            brightness        = spec.brightness;
            glowColorOverride = spec.glowColorOverride;
            ttl               = spec.ttl;
            block             = spec.block;
            return this;
        }

        public BlockDisplaySpec build() {
            if (block == null) {
                throw new IllegalArgumentException("block is required");
            }
            return new BlockDisplaySpec(this);
        }
    }

}
