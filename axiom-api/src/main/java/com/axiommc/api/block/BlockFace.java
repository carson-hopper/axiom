package com.axiommc.api.world.block;

import com.axiommc.api.math.Vector3;

public enum BlockFace {
    NORTH( 0,  0, -1),
    SOUTH( 0,  0,  1),
    EAST ( 1,  0,  0),
    WEST (-1,  0,  0),
    UP   ( 0,  1,  0),
    DOWN ( 0, -1,  0);

    private final int dx;
    private final int dy;
    private final int dz;

    BlockFace(int dx, int dy, int dz) {
        this.dx = dx;
        this.dy = dy;
        this.dz = dz;
    }

    public Vector3 direction() {
        return new Vector3(dx, dy, dz);
    }

    public BlockFace opposite() {
        return switch (this) {
            case NORTH -> SOUTH;
            case SOUTH -> NORTH;
            case EAST  -> WEST;
            case WEST  -> EAST;
            case UP    -> DOWN;
            case DOWN  -> UP;
        };
    }
}
