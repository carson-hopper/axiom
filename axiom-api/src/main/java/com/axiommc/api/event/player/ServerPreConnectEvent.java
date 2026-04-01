package com.axiommc.api.event.player;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.Server;

/**
 * Fired before a player connects to a server.
 */
public class ServerPreConnectEvent extends Event implements Cancellable {

    private final Player player;
    private Server targetServer;
    private boolean cancelled = false;

    public ServerPreConnectEvent(Player player, Server targetServer) {
        this.player = player;
        this.targetServer = targetServer;
    }

    public Player player() {
        return player;
    }

    public Server targetServer() {
        return targetServer;
    }

    public void targetServer(Server targetServer) {
        this.targetServer = targetServer;
    }

    @Override
    public boolean isCancelled() {
        return cancelled;
    }

    @Override
    public void setCancelled(boolean cancelled) {
        this.cancelled = cancelled;
    }
}
