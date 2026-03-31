package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies the valid range for a numeric command parameter.
 *
 * <p>Applied to numeric parameters to enforce minimum and maximum bounds.
 * The command system will validate that the provided value falls within
 * this range.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Range {
    /**
     * The minimum allowed value (inclusive).
     *
     * @return the minimum value
     */
    double min() default -Double.MAX_VALUE;

    /**
     * The maximum allowed value (inclusive).
     *
     * @return the maximum value
     */
    double max() default Double.MAX_VALUE;
}
