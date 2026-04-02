package com.axiommc.api.command.annotation;

import com.axiommc.api.command.SenderType;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as the command execution handler.
 *
 * <p>The annotated method will be invoked when the command is executed.
 * Use {@link #type()} to restrict which sender types can invoke this method.
 * When multiple @Execute methods exist with different types, the invoker
 * selects the most specific match for the sender.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Execute {

    /**
     * The sender type that can execute this method.
     * Defaults to BOTH (any sender).
     */
    SenderType type() default SenderType.BOTH;
}
