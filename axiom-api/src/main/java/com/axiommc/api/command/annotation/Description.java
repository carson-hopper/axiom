package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Provides a human-readable description of a command.
 *
 * <p>Applied to command classes. If not specified, no description is provided.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Description {
    /**
     * The command description.
     *
     * @return the description
     */
    String value();
}
