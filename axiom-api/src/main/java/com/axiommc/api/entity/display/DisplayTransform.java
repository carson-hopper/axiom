package com.axiommc.api.entity.display;

import com.axiommc.api.math.Vector3;

/**
 * Raw affine transform for advanced use cases. When set on a spec it
 * overrides the convenience {@code scale} and {@code yaw} fields.
 *
 * @param translation   positional offset from the entity's spawn location
 * @param leftRotation  quaternion components [x, y, z, w] applied before scale
 * @param scale         per-axis scale
 * @param rightRotation quaternion components [x, y, z, w] applied after scale
 */
public record DisplayTransform(
        Vector3 translation,
        float[] leftRotation,
        Vector3 scale,
        float[] rightRotation
) {

    public DisplayTransform {
        if (leftRotation == null || leftRotation.length != 4) {
            throw new IllegalArgumentException("leftRotation must be float[4]");
        }
        if (rightRotation == null || rightRotation.length != 4) {
            throw new IllegalArgumentException("rightRotation must be float[4]");
        }
        leftRotation  = leftRotation.clone();
        rightRotation = rightRotation.clone();
    }

}
