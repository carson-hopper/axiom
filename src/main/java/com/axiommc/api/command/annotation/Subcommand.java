package com.axiommc.api.command.annotation;

import com.axiommc.api.command.CommandSide;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as a subcommand handler.
 *
 * <p>Subcommands allow organizing related command functions under a parent
 * command. For example, {@code /admin users} and {@code /admin settings}
 * are subcommands of {@code /admin}.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Subcommand {
    /**
     * The subcommand name.
     *
     * <p>If empty, the method name is used as the subcommand name.
     *
     * @return the subcommand name
     */
    String value() default "";

    /**
     * Which side(s) this subcommand is available on.
     *
     * @return the command side (defaults to {@link CommandSide#BOTH})
     */
    CommandSide side() default CommandSide.BOTH;
}
