package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies the permission node required to execute a command method.
 *
 * <p>Can be applied at class level as a default, or on individual methods (@Execute, @Subcommand)
 * to override. Method-level permissions take precedence over class-level permissions.
 * If not specified, no permission is required.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.TYPE, ElementType.METHOD})
public @interface Permission {
    /**
     * The permission node required to execute the command method.
     *
     * @return the permission string (empty string by default, no permission needed)
     */
    String value() default "";
}
