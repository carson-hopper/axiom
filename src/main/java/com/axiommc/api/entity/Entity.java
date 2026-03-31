package com.axiommc.api.entity;

import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.world.World;

import java.util.UUID;

public interface Entity {

    UUID id();
    String name();

    String nickname();
    void nickname(String name);

    Location location();
    void teleport(Location location);

    Vector3 velocity();
    void velocity(Vector3 velocity);

    Vector2 rotation();
    void rotation(Vector2 rotation);

    World world();

    boolean alive();
}
