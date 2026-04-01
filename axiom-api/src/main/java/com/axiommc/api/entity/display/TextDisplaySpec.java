package com.axiommc.api.entity.display;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.math.Vector3;

public final class TextDisplaySpec {

    // ── Shared base fields ────────────────────────────────────────────────────
    private final Billboard         billboard;
    private final float             scale;
    private final float             yaw;
    private final Vector3           translation;
    private final DisplayTransform  rawTransform;       // nullable
    private final float             viewRange;
    private final float             shadowRadius;
    private final float             shadowStrength;
    private final DisplayBrightness brightness;         // nullable
    private final Integer           glowColorOverride;  // nullable ARGB
    private final int               ttl;                // ticks; 0 = never

    // ── Text-specific fields ──────────────────────────────────────────────────
    private final ChatComponent  text;
    private final int            lineWidth;
    private final int            backgroundColor;   // ARGB
    private final boolean        defaultBackground;
    private final byte           textOpacity;
    private final boolean        shadow;
    private final boolean        seeThrough;
    private final TextAlignment  alignment;

    private TextDisplaySpec(Builder b) {
        this.billboard          = b.billboard;
        this.scale              = b.scale;
        this.yaw                = b.yaw;
        this.translation        = b.translation;
        this.rawTransform       = b.rawTransform;
        this.viewRange          = b.viewRange;
        this.shadowRadius       = b.shadowRadius;
        this.shadowStrength     = b.shadowStrength;
        this.brightness         = b.brightness;
        this.glowColorOverride  = b.glowColorOverride;
        this.ttl                = b.ttl;
        this.text               = b.text;
        this.lineWidth          = b.lineWidth;
        this.backgroundColor    = b.backgroundColor;
        this.defaultBackground  = b.defaultBackground;
        this.textOpacity        = b.textOpacity;
        this.shadow             = b.shadow;
        this.seeThrough         = b.seeThrough;
        this.alignment          = b.alignment;
    }

    // ── Getters ───────────────────────────────────────────────────────────────
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
    public ChatComponent     text()              { return text; }
    public int               lineWidth()         { return lineWidth; }
    public int               backgroundColor()   { return backgroundColor; }
    public boolean           defaultBackground() { return defaultBackground; }
    public byte              textOpacity()       { return textOpacity; }
    public boolean           shadow()            { return shadow; }
    public boolean           seeThrough()        { return seeThrough; }
    public TextAlignment     alignment()         { return alignment; }

    /** Returns a new builder seeded with all fields from this spec. */
    public Builder toBuilder() {
        return new Builder().copyFrom(this);
    }

    /** Returns a new blank builder. */
    public static Builder spec() { return new Builder(); }

    // ── Builder ───────────────────────────────────────────────────────────────
    public static final class Builder {
        // Shared defaults
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
        // Text-specific defaults
        private ChatComponent     text             = null; // required
        private int               lineWidth        = 200;
        private int               backgroundColor  = 0x40000000;
        private boolean           defaultBackground = false;
        private byte              textOpacity      = (byte) -1;
        private boolean           shadow           = false;
        private boolean           seeThrough       = false;
        private TextAlignment     alignment        = TextAlignment.CENTER;

        // Shared setters
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
        // Text-specific setters
        public Builder text(ChatComponent t)             { text = t; return this; }
        public Builder lineWidth(int w)                  { lineWidth = w; return this; }
        public Builder backgroundColor(int argb)         { backgroundColor = argb; defaultBackground = false; return this; }
        public Builder defaultBackground(boolean b)      { defaultBackground = b; return this; }
        public Builder textOpacity(byte o)               { textOpacity = o; return this; }
        public Builder shadow(boolean s)                 { shadow = s; return this; }
        public Builder seeThrough(boolean s)             { seeThrough = s; return this; }
        public Builder alignment(TextAlignment a)        { alignment = a; return this; }

        /** Copies all fields from an existing spec (used for seeded updates). */
        public Builder copyFrom(TextDisplaySpec spec) {
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
            text              = spec.text;
            lineWidth         = spec.lineWidth;
            backgroundColor   = spec.backgroundColor;
            defaultBackground = spec.defaultBackground;
            textOpacity       = spec.textOpacity;
            shadow            = spec.shadow;
            seeThrough        = spec.seeThrough;
            alignment         = spec.alignment;
            return this;
        }

        public TextDisplaySpec build() {
            if (text == null) throw new IllegalArgumentException("text is required");
            return new TextDisplaySpec(this);
        }
    }

}
