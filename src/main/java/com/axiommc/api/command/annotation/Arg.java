package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a command method parameter as a named argument.
 *
 * <p>The value specifies the argument name used in the command syntax.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Arg {
    /**
     * The argument name in the command syntax.
     *
     * @return the argument name
     */
    String value();
}
