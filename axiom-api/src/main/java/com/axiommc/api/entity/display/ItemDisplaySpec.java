package com.axiommc.api.entity.display;

import com.axiommc.api.math.Vector3;

public final class ItemDisplaySpec {

    // Shared base fields (same set as TextDisplaySpec)
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

    // Item-specific fields
    private final String              item;           // e.g. "minecraft:diamond_sword"
    private final ItemDisplayContext  displayContext;

    private ItemDisplaySpec(Builder b) {
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
        this.item              = b.item;
        this.displayContext    = b.displayContext;
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
    public String            item()              { return item; }
    public ItemDisplayContext displayContext()   { return displayContext; }

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
        private String            item             = null; // required
        private ItemDisplayContext displayContext   = ItemDisplayContext.FIXED;

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
        public Builder item(String itemId)               { item = itemId; return this; }
        public Builder displayContext(ItemDisplayContext c) { displayContext = c; return this; }

        public Builder copyFrom(ItemDisplaySpec spec) {
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
            item              = spec.item;
            displayContext    = spec.displayContext;
            return this;
        }

        public ItemDisplaySpec build() {
            if (item == null) {
                throw new IllegalArgumentException("item is required");
            }
            return new ItemDisplaySpec(this);
        }
    }

}
