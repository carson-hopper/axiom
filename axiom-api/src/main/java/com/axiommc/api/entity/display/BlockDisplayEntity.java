package com.axiommc.api.entity.display;

import java.util.function.Consumer;

/** A block display entity handle. */
public interface BlockDisplayEntity extends DisplayEntity {

    void update(Consumer<BlockDisplaySpec.Builder> editor, int interpolationTicks);

    default void update(Consumer<BlockDisplaySpec.Builder> editor) {
        update(editor, 0);
    }
}
