package com.axiommc.api.event.vehicle;

import com.axiommc.api.entity.Entity;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.World;

/**
 * Events related to vehicles.
 *
 * <ul>
 *   <li>{@link Enter} — player enters a vehicle; cancellable
 *   <li>{@link Exit} — player exits a vehicle
 *   <li>{@link Create} — vehicle is created; cancellable
 *   <li>{@link Destroy} — vehicle is destroyed; cancellable
 *   <li>{@link Damage} — vehicle takes damage; cancellable
 *   <li>{@link Move} — vehicle moves
 *   <li>{@link Collision} — vehicle collides with entity or block
 * </ul>
 */
public final class VehicleEvent {

    private VehicleEvent() {}

    /** Fired when a player enters a vehicle. */
    public static class Enter extends Event implements Cancellable {

        private final Player player;
        private final Entity vehicle;
        private boolean cancelled = false;

        public Enter(Player player, Entity vehicle) {
            this.player = player;
            this.vehicle = vehicle;
        }

        public Player player() {
            return player;
        }

        public Entity vehicle() {
            return vehicle;
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

    /** Fired when a player exits a vehicle. */
    public static class Exit extends Event {

        private final Player player;
        private final Entity vehicle;

        public Exit(Player player, Entity vehicle) {
            this.player = player;
            this.vehicle = vehicle;
        }

        public Player player() {
            return player;
        }

        public Entity vehicle() {
            return vehicle;
        }
    }

    /** Fired when a vehicle is created. */
    public static class Create extends Event implements Cancellable {

        private final Entity vehicle;
        private final World world;
        private boolean cancelled = false;

        public Create(Entity vehicle, World world) {
            this.vehicle = vehicle;
            this.world = world;
        }

        public Entity vehicle() {
            return vehicle;
        }

        public World world() {
            return world;
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

    /** Fired when a vehicle is destroyed. Attacker may be null. */
    public static class Destroy extends Event implements Cancellable {

        private final Entity vehicle;
        private final Entity attacker;
        private boolean cancelled = false;

        public Destroy(Entity vehicle, Entity attacker) {
            this.vehicle = vehicle;
            this.attacker = attacker;
        }

        public Entity vehicle() {
            return vehicle;
        }

        public Entity attacker() {
            return attacker;
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

    /** Fired when a vehicle takes damage. Attacker may be null. */
    public static class Damage extends Event implements Cancellable {

        private final Entity vehicle;
        private final Entity attacker;
        private final double damage;
        private boolean cancelled = false;

        public Damage(Entity vehicle, Entity attacker, double damage) {
            this.vehicle = vehicle;
            this.attacker = attacker;
            this.damage = damage;
        }

        public Entity vehicle() {
            return vehicle;
        }

        public Entity attacker() {
            return attacker;
        }

        public double damage() {
            return damage;
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

    /** Fired when a vehicle moves. */
    public static class Move extends Event {

        private final Entity vehicle;
        private final Location from;
        private final Location to;

        public Move(Entity vehicle, Location from, Location to) {
            this.vehicle = vehicle;
            this.from = from;
            this.to = to;
        }

        public Entity vehicle() {
            return vehicle;
        }

        public Location from() {
            return from;
        }

        public Location to() {
            return to;
        }
    }

    /** Fired when a vehicle collides. hitEntity may be null for block collisions. */
    public static class Collision extends Event {

        private final Entity vehicle;
        private final Entity hitEntity;
        private final Location hitLocation;

        public Collision(Entity vehicle, Entity hitEntity, Location hitLocation) {
            this.vehicle = vehicle;
            this.hitEntity = hitEntity;
            this.hitLocation = hitLocation;
        }

        public Entity vehicle() {
            return vehicle;
        }

        public Entity hitEntity() {
            return hitEntity;
        }

        public Location hitLocation() {
            return hitLocation;
        }
    }
}
