package com.axiommc.api.player;

import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;

public record Location(World world, Vector3 position, Vector2 rotation) {
}
