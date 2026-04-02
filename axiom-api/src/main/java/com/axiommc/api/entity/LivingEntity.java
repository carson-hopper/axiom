package com.axiommc.api.entity;

/**
 * Represents an entity that can take damage and be healed.
 *
 * <p>Living entities have health points and can be damaged or healed.
 * This interface extends {@link Entity} with health-related methods.
 */
public interface LivingEntity extends Entity {

    /**
     * Gets the current health of this entity.
     *
     * @return the current health
     */
    double health();

    /**
     * Sets the health of this entity.
     *
     * @param health the new health value
     */
    void health(double health);

    /**
     * Gets the maximum health of this entity.
     *
     * @return the maximum health
     */
    double maxHealth();

    /**
     * Damages this entity.
     *
     * @param amount the amount of damage to deal (in half-hearts)
     */
    void damage(double amount);

    /**
     * Kills this entity immediately, bypassing invulnerability and armor.
     */
    void kill();
}
