package com.axiommc.api.command.annotation;

import com.axiommc.api.command.CommandSide;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Metadata for a command class.
 *
 * <p>Applied to command classes to define the command name, aliases, description,
 * permission requirement, and which side (client/server) the command runs on.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface CommandMeta {
    /**
     * The primary command name.
     *
     * @return the command name
     */
    String name();

    /**
     * Alternative names for the command.
     *
     * @return the command aliases (empty array by default)
     */
    String[] aliases() default {};

    /**
     * A human-readable description of the command.
     *
     * @return the command description (empty string by default)
     */
    String description() default "";

    /**
     * The permission node required to execute the command.
     *
     * @return the permission string (empty string by default, no permission needed)
     */
    String permission() default "";

    /**
     * Which side(s) this command executes on.
     *
     * @return the command side (defaults to {@link CommandSide#BOTH})
     */
    CommandSide side() default CommandSide.BOTH;
}
