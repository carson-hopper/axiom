package com.axiommc.api.event.entity;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;

public class EntityEvent {

    /**
     * Fired when an entity spawns.
     */
    public static class Spawn extends Event implements Cancellable {

        private final Entity entity;
        private final World world;
        private final Location location;
        private final String reason;
        private boolean cancelled = false;

        public Spawn(Entity entity, World world, Location location, String reason) {
            this.entity = entity;
            this.world = world;
            this.location = location;
            this.reason = reason;
        }

        public Entity entity() {
            return entity;
        }

        public World world() {
            return world;
        }

        public Location location() {
            return location;
        }

        public String reason() {
            return reason;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when an entity dies.
     */
    public static class Death extends Event {

        private final Entity entity;

        public Death(Entity entity) {
            this.entity = entity;
        }

        public Entity entity() {
            return entity;
        }
    }

    /**
     * Fired when an entity takes damage.
     */
    public static class Damage extends Event implements Cancellable {

        private final Entity entity;
        private final double damage;
        private final String cause;
        private boolean cancelled = false;

        public Damage(Entity entity, double damage, String cause) {
            this.entity = entity;
            this.damage = damage;
            this.cause = cause;
        }

        public Entity entity() {
            return entity;
        }

        public double damage() {
            return damage;
        }

        public String cause() {
            return cause;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when an entity explodes.
     */
    public static class Explode extends Event implements Cancellable {

        private final Entity entity;
        private final Location location;
        private boolean cancelled = false;

        public Explode(Entity entity, Location location) {
            this.entity = entity;
            this.location = location;
        }

        public Entity entity() {
            return entity;
        }

        public Location location() {
            return location;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when a mob targets an entity.
     */
    public static class Target extends Event implements Cancellable {

        private final Entity entity;
        private final Entity target;
        private boolean cancelled = false;

        public Target(Entity entity, Entity target) {
            this.entity = entity;
            this.target = target;
        }

        public Entity entity() {
            return entity;
        }

        public Entity target() {
            return target;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when a projectile is launched.
     */
    public static class ProjectileLaunch extends Event implements Cancellable {

        private final Entity entity;
        private boolean cancelled = false;

        public ProjectileLaunch(Entity entity) {
            this.entity = entity;
        }

        public Entity entity() {
            return entity;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    /**
     * Fired when a projectile hits something. hitEntity may be null.
     */
    public static class ProjectileHit extends Event {

        private final Entity entity;
        private final Entity hitEntity;
        private final Location hitLocation;

        public ProjectileHit(Entity entity, Entity hitEntity, Location hitLocation) {
            this.entity = entity;
            this.hitEntity = hitEntity;
            this.hitLocation = hitLocation;
        }

        public Entity entity() {
            return entity;
        }

        public Entity hitEntity() {
            return hitEntity;
        }

        public Location hitLocation() {
            return hitLocation;
        }
    }

    /**
     * Fired when an entity is tamed.
     */
    public static class Tame extends Event {

        private final Entity entity;
        private final Player owner;

        public Tame(Entity entity, Player owner) {
            this.entity = entity;
            this.owner = owner;
        }

        public Entity entity() {
            return entity;
        }

        public Player owner() {
            return owner;
        }
    }

    /**
     * Fired when entities breed.
     */
    public static class Breed extends Event {

        private final Entity parent1;
        private final Entity parent2;
        private final Entity child;

        public Breed(Entity parent1, Entity parent2, Entity child) {
            this.parent1 = parent1;
            this.parent2 = parent2;
            this.child = child;
        }

        public Entity parent1() {
            return parent1;
        }

        public Entity parent2() {
            return parent2;
        }

        public Entity child() {
            return child;
        }
    }
}
