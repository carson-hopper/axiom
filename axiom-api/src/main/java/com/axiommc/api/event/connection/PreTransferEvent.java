package com.axiommc.api.event.connection;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.Server;

/**
 * Fired before a player is transferred to another server.
 * Can be cancelled to prevent the transfer.
 */
public class PreTransferEvent extends Event implements Cancellable {

    private final Player player;
    private final Server targetServer;
    private boolean cancelled = false;

    public PreTransferEvent(Player player, Server targetServer) {
        this.player = player;
        this.targetServer = targetServer;
    }

    public Player player() {
        return player;
    }

    public Server targetServer() {
        return targetServer;
    }

    @Override
    public boolean isCancelled() {
        return cancelled;
    }

    @Override
    public void cancelled(boolean cancelled) {
        this.cancelled = cancelled;
    }
}
