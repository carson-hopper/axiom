package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Provides static tab completion suggestions for a command parameter.
 *
 * <p>The specified values are offered as completions when the player presses
 * the tab key to auto-complete the parameter. Use
 * {@link DynamicTabComplete} for runtime-generated suggestions.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface TabComplete {
    /**
     * The list of static completion suggestions.
     *
     * @return the completion options
     */
    String[] value();
}
