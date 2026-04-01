package com.axiommc.api.command;

/**
 * Indicates which side a command executes on.
 */
public enum CommandSide {
    /**
     * Command executes on the proxy side (client-side).
     */
    PROXY,

    /**
     * Command executes on the server side.
     */
    SERVER,

    /**
     * Command executes on both proxy and server sides.
     */
    BOTH
}
