package com.axiommc.api.plugin;

/**
 * Indicates which side a plugin runs on.
 */
public enum PluginSide {
    /**
     * Plugin runs on server side only.
     */
    SERVER,

    /**
     * Plugin runs on client side only.
     */
    CLIENT,

    /**
     * Plugin runs on both server and client.
     */
    BOTH
}
