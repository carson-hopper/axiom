package com.axiommc.api.particle;

public abstract sealed class ParticleData
        permits DustData, DustTransitionData, BlockData, ItemData, ColorData {}
