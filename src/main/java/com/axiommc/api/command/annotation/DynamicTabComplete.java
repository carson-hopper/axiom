package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a parameter to use dynamic tab completion from a method.
 *
 * <p>The referenced method should be on the Command class and have the signature:
 * <pre>{@code List<String> methodName(String partial)}</pre>
 *
 * <p>The method will be called to generate tab completion suggestions matching
 * the partial input from the user.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface DynamicTabComplete {
    /**
     * The name of the method that provides tab completion suggestions.
     *
     * @return the method name
     */
    String value();
}
