package com.axiommc.api.entity.display;

import java.util.function.Consumer;

/**
 * A text display entity handle. Use {@link #update} to push partial
 * changes with an optional client-side interpolation transition.
 */
public interface TextDisplayEntity extends DisplayEntity {

    /**
     * Applies partial changes to this display with a smooth client-side transition.
     * The builder is pre-seeded from the current spec so only changed fields need
     * to be set.
     *
     * @param editor             consumer that modifies the builder
     * @param interpolationTicks client-side transition duration in ticks (0 = instant)
     */
    void update(Consumer<TextDisplaySpec.Builder> editor, int interpolationTicks);

    /** Instant update — equivalent to {@code update(editor, 0)}. */
    default void update(Consumer<TextDisplaySpec.Builder> editor) {
        update(editor, 0);
    }
}
