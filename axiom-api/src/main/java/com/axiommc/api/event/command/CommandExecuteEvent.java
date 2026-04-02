package com.axiommc.api.event.command;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;

/**
 * Events fired when a command is executed.
 *
 * <ul>
 *   <li>{@link Pre} — before the command runs; cancellable
 *   <li>{@link Post} — after the command has executed
 * </ul>
 */
public final class CommandExecuteEvent {

    private CommandExecuteEvent() {}

    /**
     * Fired before a command is executed. Cancel to prevent execution.
     */
    public static class Pre extends Event implements Cancellable {

        private final CommandSender sender;
        private final String command;
        private boolean cancelled = false;

        public Pre(CommandSender sender, String command) {
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

    /**
     * Fired after a command has executed.
     */
    public static class Post extends Event {

        private final CommandSender sender;
        private final String command;

        public Post(CommandSender sender, String command) {
            this.sender = sender;
            this.command = command;
        }

        public CommandSender sender() {
            return sender;
        }

        public String command() {
            return command;
        }
    }
}
