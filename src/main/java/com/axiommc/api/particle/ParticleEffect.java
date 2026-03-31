package com.axiommc.api.particle;

import java.util.Objects;

public record ParticleEffect(ParticleType type, ParticleData data, int count, double spreadX, double spreadY,
                             double spreadZ, float speed, boolean force) {

    // ============================================================
    // Record Accessors
    // ============================================================

    /**
     * Null for no-data particle types.
     */
    @Override
    public ParticleData data() {
        return data;
    }

    /**
     * When true, the particle packet bypasses normal view-distance culling.
     */
    @Override
    public boolean force() {
        return force;
    }

    // ============================================================
    // Builder
    // ============================================================

    public static final class Builder {

        private final ParticleType type;
        private ParticleData data;
        private int count = 1;
        private double spreadX, spreadY, spreadZ;
        private float speed;
        private boolean force;

        // ────────────────────────────────────────────────────────
        // Initialization
        // ────────────────────────────────────────────────────────

        public Builder(ParticleType type) {
            this.type = Objects.requireNonNull(type, "type");
        }

        // ────────────────────────────────────────────────────────
        // Data Configuration
        // ────────────────────────────────────────────────────────

        /**
         * Sets particle data. Validates immediately that the data type matches
         * what this particle type declares.
         *
         * @throws IllegalArgumentException if the particle takes no data, or if
         *                                  the supplied data is the wrong type
         */
        public Builder data(ParticleData data) {
            Objects.requireNonNull(data, "data");
            validateParticleData(data);
            this.data = data;
            return this;
        }

        private void validateParticleData(ParticleData data) {
            Class<?> expected = type.dataType();
            if (expected == null) {
                throw new IllegalArgumentException(type.key() + " does not accept data");
            }
            if (!expected.isInstance(data)) {
                throw new IllegalArgumentException(
                        type.key() + " requires " + expected.getSimpleName()
                                + ", got " + data.getClass().getSimpleName());
            }
        }

        // ────────────────────────────────────────────────────────
        // Effect Configuration
        // ────────────────────────────────────────────────────────

        public Builder count(int count) {
            if (count < 0) throw new IllegalArgumentException("count must be >= 0");
            this.count = count;
            return this;
        }

        public Builder spread(double x, double y, double z) {
            this.spreadX = x;
            this.spreadY = y;
            this.spreadZ = z;
            return this;
        }

        public Builder speed(float speed) {
            this.speed = speed;
            return this;
        }

        /**
         * When {@code true}, the particle packet is sent even beyond normal view distance.
         */
        public Builder force(boolean force) {
            this.force = force;
            return this;
        }

        // ────────────────────────────────────────────────────────
        // Build & Validation
        // ────────────────────────────────────────────────────────

        /**
         * Builds the effect. Validates that data-requiring particles have data set.
         *
         * @throws IllegalArgumentException if required data is missing
         */
        public ParticleEffect build() {
            validateParticleDataRequired();
            return new ParticleEffect(type, data, count, spreadX, spreadY, spreadZ, speed, force);
        }

        private void validateParticleDataRequired() {
            Class<?> required = type.dataType();
            if (required == null) return;
            if (data != null) return;
            throw new IllegalArgumentException(
                    type.key() + " requires " + required.getSimpleName() + " data");
        }
    }
}
