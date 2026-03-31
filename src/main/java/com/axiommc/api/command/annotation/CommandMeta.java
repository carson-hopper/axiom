package com.axiommc.api.command.annotation;

import com.axiommc.api.command.CommandSide;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface CommandMeta {
    String name();
    String[] aliases() default {};
    String description() default "";
    String permission() default "";
    CommandSide side() default CommandSide.BOTH;
}
