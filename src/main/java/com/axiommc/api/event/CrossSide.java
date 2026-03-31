package com.axiommc.api.event;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks an event class for automatic cross-side forwarding.
 *
 * <p>When a {@code @CrossSide} event is published via {@link EventBus#publish(Event)},
 * the framework serializes it and delivers it to the other side's event bus automatically.
 *
 * <p>Constraints:
 * <ul>
 *   <li>Must have a no-arg constructor (package-private is fine).</li>
 *   <li>All non-static, non-transient fields must be wire-safe types:
 *       primitives, {@link String}, {@link java.util.UUID}, or {@link Enum} subtypes.</li>
 *   <li>Use {@code UUID} for player references — {@code Player} cannot cross the wire.</li>
 * </ul>
 */
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
public @interface CrossSide {}
