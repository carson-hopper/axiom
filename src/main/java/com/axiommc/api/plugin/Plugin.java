package com.axiommc.api.plugin;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Metadata annotation for a plugin class.
 *
 * <p>Applied to a plugin's main class to define its identity, version,
 * execution side (client/server), and description.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface Plugin {
    /**
     * The unique plugin identifier.
     *
     * <p>Should be lowercase with hyphens or underscores (e.g., "my-plugin").
     *
     * @return the plugin ID
     */
    String id();

    /**
     * The human-readable plugin name.
     *
     * @return the plugin name
     */
    String name();

    /**
     * The plugin version.
     *
     * <p>Should follow semantic versioning (e.g., "1.0.0").
     *
     * @return the version string
     */
    String version();

    /**
     * Which side(s) this plugin runs on.
     *
     * @return the plugin side
     */
    PluginSide side();

    /**
     * A description of what the plugin does.
     *
     * @return the plugin description (empty string by default)
     */
    String description() default "";
}
