package com.axiommc.api.entity;

import com.axiommc.api.item.ItemStack;

/**
 * Represents a dropped item entity in the world.
 *
 * <p>An item entity is the physical representation of an {@link ItemStack}
 * that exists in the world after being dropped by a player, spawned by
 * a block, or created by other means.
 */
public interface ItemEntity extends Entity {

    /**
     * Returns the item stack this entity represents.
     *
     * @return the item stack
     */
    ItemStack itemStack();

    /**
     * Returns the remaining ticks before this item can be picked up.
     *
     * @return the pickup delay in ticks
     */
    int pickupDelay();

    /**
     * Sets the pickup delay for this item.
     *
     * @param ticks the delay in ticks before the item can be picked up
     */
    void pickupDelay(int ticks);
}
