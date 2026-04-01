package com.axiommc.api.event.plugin;

import com.axiommc.api.event.Event;

/**
 * Event fired when a plugin's state changes.
 */
public class PluginStateEvent extends Event {
    public enum State {
        ENABLED,
        DISABLED
    }

    private final String pluginId;
    private final State state;
    private final String failureReason;

    public PluginStateEvent(String pluginId, State state) {
        this(pluginId, state, "");
    }

    public PluginStateEvent(String pluginId, State state, String failureReason) {
        this.pluginId = pluginId;
        this.state = state;
        this.failureReason = failureReason != null ? failureReason : "";
    }

    public String pluginId() {
        return pluginId;
    }

    public State state() {
        return state;
    }

    public String failureReason() {
        return failureReason;
    }
}
