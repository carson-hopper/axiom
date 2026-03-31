package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a parameter to use dynamic tab completion from a method.
 * The method should be on the Command class, have signature:
 *   List<String> methodName(String partial)
 * and return suggestions matching the partial input.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface DynamicTabComplete {
    String value();
}
