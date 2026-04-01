package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a class as a command.
 *
 * <p>Applied to command classes to define the command name and aliases.
 * Use {@link Description}, {@link Permission}, and {@link Side} annotations
 * for additional metadata.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Command {
    /**
     * The primary command name.
     *
     * @return the command name
     */
    String name();

    /**
     * Alternative names for the command.
     *
     * @return the command aliases (empty array by default)
     */
    String[] aliases() default {};
}
