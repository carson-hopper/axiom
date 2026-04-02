package com.axiommc.api.command.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a command parameter as greedy.
 *
 * <p>A greedy parameter consumes all remaining arguments as a single string.
 * Useful for multi-word arguments like messages or descriptions.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.PARAMETER)
public @interface Greedy {}
