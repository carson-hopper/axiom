package com.axiommc.fabric.particle;

import com.axiommc.api.particle.ParticleEffect;
import net.minecraft.core.particles.ParticleOptions;
import net.minecraft.core.particles.ParticleTypes;
import net.minecraft.core.particles.SimpleParticleType;
import com.axiommc.fabric.Axiom;

public class FabricParticleConverter {

    public static ParticleOptions toMinecraftParticle(ParticleEffect effect) {
        String key = effect.type().key();

        try {
            // Map Axiom particle keys to Minecraft ParticleType
            ParticleOptions particleOptions = mapParticle(key);

            if (particleOptions == null) {
                Axiom.logger().warn("Unknown or unsupported particle type: {}", key);
                return null;
            }

            // Check if data is required but not provided
            if (effect.data() != null && !(particleOptions instanceof SimpleParticleType)) {
                // Complex particle types with data are not yet supported
                Axiom.logger().warn("Particle type {} with data is not yet supported", key);
                return null;
            }

            return particleOptions;

        } catch (Exception e) {
            Axiom.logger().error("Failed to convert particle: {}", key, e);
            return null;
        }
    }

    private static ParticleOptions mapParticle(String key) {
        return switch (key) {
            // No-data particles
            case "minecraft:flame" -> ParticleTypes.FLAME;
            case "minecraft:smoke" -> ParticleTypes.SMOKE;
            case "minecraft:large_smoke" -> ParticleTypes.LARGE_SMOKE;
            case "minecraft:explosion" -> ParticleTypes.EXPLOSION;
            case "minecraft:explosion_emitter" -> ParticleTypes.EXPLOSION_EMITTER;
            case "minecraft:crit" -> ParticleTypes.CRIT;
            case "minecraft:enchanted_hit" -> ParticleTypes.ENCHANTED_HIT;
            case "minecraft:happy_villager" -> ParticleTypes.HAPPY_VILLAGER;
            case "minecraft:angry_villager" -> ParticleTypes.ANGRY_VILLAGER;
            case "minecraft:heart" -> ParticleTypes.HEART;
            case "minecraft:portal" -> ParticleTypes.PORTAL;
            case "minecraft:end_rod" -> ParticleTypes.END_ROD;
            case "minecraft:witch" -> ParticleTypes.WITCH;
            case "minecraft:dripping_water" -> ParticleTypes.DRIPPING_WATER;
            case "minecraft:dripping_lava" -> ParticleTypes.DRIPPING_LAVA;
            case "minecraft:snowflake" -> ParticleTypes.SNOWFLAKE;
            case "minecraft:totem_of_undying" -> ParticleTypes.TOTEM_OF_UNDYING;
            case "minecraft:note" -> ParticleTypes.NOTE;
            case "minecraft:bubble" -> ParticleTypes.BUBBLE;
            case "minecraft:splash" -> ParticleTypes.SPLASH;

            // TODO: Data particles (dust, block, item, entity_effect)
            default -> null;
        };
    }
}
