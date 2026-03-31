package com.axiommc.api.command.annotation;

import com.axiommc.api.command.CommandSide;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Subcommand {
    /** Subcommand name. If empty, uses the method name. */
    String value() default "";
    /** Which side(s) this subcommand is available on. Defaults to {@link CommandSide#BOTH}. */
    CommandSide side() default CommandSide.BOTH;
}
