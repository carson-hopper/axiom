package com.axiommc.api.plugin;

/**
 * Represents the environment/side where a plugin runs.
 * SERVER = running on a game server
 * PROXY = running on a proxy server
 * BOTH = can run on either
 */
public enum PluginEnvironment {
    SERVER,
    PROXY,
    BOTH
}
