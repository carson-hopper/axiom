package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies a default value for an optional command parameter.
 *
 * <p>If the parameter is not provided by the user, this default value is used.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Default {
    /**
     * The default value as a string.
     *
     * @return the default value
     */
    String value();
}
