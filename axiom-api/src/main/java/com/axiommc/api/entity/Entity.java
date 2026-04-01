package com.axiommc.api.entity;

import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;

import java.util.UUID;

/**
 * Represents an entity in the world.
 *
 * <p>An entity is any object that exists in a world, such as mobs, items,
 * or armor stands. Entities have a position, velocity, rotation, and unique
 * identifier.
 */
public interface Entity {

    /**
     * Gets the unique identifier of this entity.
     *
     * @return the entity UUID
     */
    UUID id();

    /**
     * Gets the entity type name.
     *
     * @return the entity type
     */
    String name();

    /**
     * Gets the custom display name of this entity.
     *
     * @return the custom nickname or empty string if not set
     */
    String nickname();

    /**
     * Sets a custom display name for this entity.
     *
     * @param name the nickname or empty string to remove
     */
    void nickname(String name);

    /**
     * Gets the location of this entity.
     *
     * @return the entity's location
     */
    Location location();

    /**
     * Teleports this entity to a location.
     *
     * @param location the destination location
     */
    void teleport(Location location);

    /**
     * Gets the velocity of this entity.
     *
     * @return the velocity vector
     */
    Vector3 velocity();

    /**
     * Sets the velocity of this entity.
     *
     * @param velocity the new velocity
     */
    void velocity(Vector3 velocity);

    /**
     * Gets the rotation (yaw and pitch) of this entity.
     *
     * @return the rotation vector (x = yaw, y = pitch)
     */
    Vector2 rotation();

    /**
     * Sets the rotation of this entity.
     *
     * @param rotation the new rotation (x = yaw, y = pitch)
     */
    void rotation(Vector2 rotation);

    /**
     * Gets the world this entity is in.
     *
     * @return the world
     */
    World world();

    /**
     * Checks whether this entity is alive.
     *
     * @return true if the entity is alive, false if dead or removed
     */
    boolean alive();
}
