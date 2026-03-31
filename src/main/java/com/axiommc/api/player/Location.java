package com.axiommc.api.player;

import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;

public record Location(World world, Vector3 position, Vector2 rotation) {

    public double distance(Location other) {
        return position.distance(other.position);
    }

    public double distance(Vector3 other) {
        return position.distance(other);
    }

    public boolean isCloseTo(Location other, double radius) {
        if (!world.equals(other.world)) {
            return false;
        }
        return position.isCloseTo(other.position, radius);
    }

    public boolean isCloseTo(Vector3 other, double radius) {
        return position.isCloseTo(other, radius);
    }
}
