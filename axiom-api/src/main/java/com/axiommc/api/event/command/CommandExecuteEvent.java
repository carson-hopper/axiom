package com.axiommc.api.event.command;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;

/**
 * Fired when a command is executed.
 */
public class CommandExecuteEvent extends Event implements Cancellable {

    private final CommandSender sender;
    private final String command;
    private boolean cancelled = false;

    public CommandExecuteEvent(CommandSender sender, String command) {
        this.sender = sender;
        this.command = command;
    }

    public CommandSender sender() {
        return sender;
    }

    public String command() {
        return command;
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
