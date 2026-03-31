package com.axiommc.api.world;

/**
 * Represents a Minecraft server in the cluster.
 * Each server knows how to reach all other servers via hostname and port.
 */
public record Server(String id, String host, int port) {

    public String id() { return id; }

    public String address() {
        return host + ":" + port;
    }

    @Override
    public String toString() {
        return id + " (" + host + ":" + port + ")";
    }
}
