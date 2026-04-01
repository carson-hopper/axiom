package com.axiommc.fabric.event.adapter;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.event.SimpleEventBus;
import com.axiommc.api.event.command.CommandExecuteEvent;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.player.FabricPlayerProvider;

/**
 * Fires CommandExecuteEvent before command execution.
 * This adapter is called directly from FabricCommandAdapter rather than
 * hooking a Fabric callback, since commands are already routed through our system.
 */
public class CommandExecuteAdapter implements FabricEventAdapter {

    private static SimpleEventBus eventBus;

    @Override
    public void register(SimpleEventBus eventBus, FabricPlayerProvider playerProvider) {
        CommandExecuteAdapter.eventBus = eventBus;
    }

    /**
     * Fires a CommandExecuteEvent and returns whether the command should proceed.
     *
     * @param sender the command sender
     * @param command the full command string
     * @return true if the command should execute, false if cancelled
     */
    public static boolean fireEvent(CommandSender sender, String command) {
        if (eventBus == null) {
            return true;
        }

        try {
            CommandExecuteEvent event = new CommandExecuteEvent(sender, command);
            eventBus.publish(event);
            return !event.isCancelled();
        } catch (Exception e) {
            Axiom.logger().debug("Error firing CommandExecuteEvent", e);
            return true;
        }
    }
}
