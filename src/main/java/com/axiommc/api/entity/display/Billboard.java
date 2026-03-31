package com.axiommc.api.entity.display;

public enum Billboard {
    /** Fixed orientation — never rotates. */
    FIXED,
    /** Rotates only around the vertical (Y) axis to face the camera. */
    VERTICAL,
    /** Rotates only around the horizontal axis. */
    HORIZONTAL,
    /** Always faces the camera (full spherical billboard). */
    CENTER
}
