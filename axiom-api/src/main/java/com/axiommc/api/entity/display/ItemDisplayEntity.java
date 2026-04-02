package com.axiommc.api.entity.display;

import java.util.function.Consumer;

/** An item display entity handle. */
public interface ItemDisplayEntity extends DisplayEntity {

    void update(Consumer<ItemDisplaySpec.Builder> editor, int interpolationTicks);

    default void update(Consumer<ItemDisplaySpec.Builder> editor) {
        update(editor, 0);
    }
}
