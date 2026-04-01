package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies the permission node required to execute a command.
 *
 * <p>Applied to command classes. If not specified, no permission is required.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Permission {
    /**
     * The permission node required to execute the command.
     *
     * @return the permission string (empty string by default, no permission needed)
     */
    String value() default "";
}
