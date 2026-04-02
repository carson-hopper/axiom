package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a command parameter as optional.
 *
 * <p>Optional parameters may be omitted when executing the command.
 * Pair with {@link Default} to provide a default value if not supplied.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Optional {}
