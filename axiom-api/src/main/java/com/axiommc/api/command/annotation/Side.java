package com.axiommc.api.command.annotation;

import com.axiommc.api.command.CommandSide;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Specifies which side(s) a command executes on.
 *
 * <p>Applied to command classes. Defaults to both client and server.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Side {
    /**
     * Which side(s) this command executes on.
     *
     * @return the command side (defaults to {@link CommandSide#BOTH})
     */
    CommandSide value() default CommandSide.BOTH;
}
