package com.axiommc.api.event.annotation;

import com.axiommc.api.event.EventPriority;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as an event handler.
 * The method must have exactly one parameter of type Event or a subclass.
 * The method must be in a class that implements EventListener.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface EventHandler {

    /**
     * The priority for this event handler.
     * Higher priority handlers are called first.
     */
    EventPriority priority() default EventPriority.NORMAL;
}
