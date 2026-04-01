package com.axiommc.api.plugin;

import com.axiommc.api.config.PluginConfig;
import org.slf4j.Logger;

import java.io.File;

/**
 * AxiomPlugin base class that all Axiom plugins must extend.
 * Plugins are modules loaded dynamically at runtime that can hook into Axiom events.
 */
public abstract class AxiomPlugin {

    protected PluginContext context;

    public final void enable(PluginContext context) {
        this.context = context;
        onEnable(context);
    }

    protected abstract void onEnable(PluginContext context);

    public abstract void onDisable();

    /**
     * Get the plugin configuration.
     */
    public final PluginConfig config() {
        if (context == null) {
            throw new IllegalStateException("Plugin not enabled");
        }
        return context.config();
    }

    /**
     * Get the plugin data folder.
     */
    public final File dataFolder() {
        if (context == null) {
            throw new IllegalStateException("Plugin not enabled");
        }
        return context.dataFolder();
    }

    /**
     * Get the plugin logger.
     */
    public final Logger logger() {
        if (context == null) {
            throw new IllegalStateException("Plugin not enabled");
        }
        return context.logger();
    }
}
