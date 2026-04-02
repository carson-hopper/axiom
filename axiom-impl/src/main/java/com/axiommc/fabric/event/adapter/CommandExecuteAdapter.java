package com.axiommc.fabric.event.adapter;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.command.CommandExecuteEvent;
import com.axiommc.api.event.server.TabCompleteEvent;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayerProvider;
import java.util.Collections;
import java.util.List;

/**
 * Fires {@link CommandExecuteEvent.Pre} and {@link CommandExecuteEvent.Post}
 * around command execution. Called directly from FabricCommandAdapter.
 */
public class CommandExecuteAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        CommandExecuteAdapter.eventBus = eventBus;
    }

    /**
     * Fires a {@link CommandExecuteEvent.Pre} and returns whether the command should proceed.
     *
     * @param sender  the command sender
     * @param command the full command string
     * @return true if the command should execute, false if cancelled
     */
    public static boolean firePreEvent(CommandSender sender, String command) {
        if (eventBus == null) {
            return true;
        }
        try {
            CommandExecuteEvent.Pre event = new CommandExecuteEvent.Pre(sender, command);
            eventBus.publish(event);
            return !event.isCancelled();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing CommandExecuteEvent.Pre", exception);
            return true;
        }
    }

    /**
     * Fires a {@link CommandExecuteEvent.Post} after command execution.
     *
     * @param sender  the command sender
     * @param command the full command string
     */
    public static void firePostEvent(CommandSender sender, String command) {
        if (eventBus == null) {
            return;
        }
        try {
            eventBus.publish(new CommandExecuteEvent.Post(sender, command));
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing CommandExecuteEvent.Post", exception);
        }
    }

    /**
     * Fires a {@link TabCompleteEvent} and returns the (possibly modified) completions.
     * If the event is cancelled, returns an empty list.
     *
     * @param sender      the command sender
     * @param buffer      the current input buffer
     * @param completions the computed completions
     * @return the completions to use, or empty if cancelled
     */
    public static List<String> fireTabComplete(
        CommandSender sender, String buffer, List<String> completions) {
        if (eventBus == null) {
            return completions;
        }
        try {
            Player player = sender.asPlayer().orElse(null);
            if (player == null) {
                return completions;
            }
            TabCompleteEvent event = new TabCompleteEvent(player, buffer, completions);
            eventBus.publish(event);
            if (event.isCancelled()) {
                return Collections.emptyList();
            }
            return event.completions();
        } catch (Exception exception) {
            Axiom.logger().debug("Error firing TabCompleteEvent", exception);
            return completions;
        }
    }
}
