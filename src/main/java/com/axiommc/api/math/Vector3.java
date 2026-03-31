package com.axiommc.api.math;

public record Vector3(double x, double y, double z) {

    public static final Vector3 ZERO = new Vector3(0, 0, 0);

    public Vector3 add(Vector3 other) {
        return new Vector3(x + other.x, y + other.y, z + other.z);
    }

    public Vector3 multiply(double factor) {
        return new Vector3(x * factor, y * factor, z * factor);
    }

    public double length() {
        return Math.sqrt(x * x + y * y + z * z);
    }

    public double lengthSquared() {
        return x * x + y * y + z * z;
    }

    public double distance(Vector3 other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }

    public double distanceSquared(Vector3 other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    public boolean isCloseTo(Vector3 other, double radius) {
        return distance(other) <= radius;
    }
}
