package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies the usage syntax for a command.
 *
 * <p>Applied to command classes. If not specified, no usage information is provided.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Usage {
    /**
     * The command usage syntax.
     *
     * @return the usage string
     */
    String value();
}
