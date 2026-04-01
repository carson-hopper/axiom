package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a command method parameter as a flag (boolean option).
 *
 * <p>The value specifies the flag name used in the command syntax.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Flag {
    /**
     * The flag name in the command syntax.
     *
     * @return the flag name
     */
    String value();
}
