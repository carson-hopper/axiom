package com.axiommc.fabric.command;

import com.axiommc.api.command.CommandRegistry;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.invoker.CommandInvoker;
import com.axiommc.api.command.parser.ArgParserRegistry;
import com.axiommc.api.plugin.PluginEnvironment;
import com.axiommc.fabric.Axiom;

import java.util.HashMap;
import java.util.Map;

public class FabricCommandRegistry implements CommandRegistry {

    private final ArgParserRegistry parserRegistry;
    private final PluginEnvironment environment;
    private final Map<String, CommandInvoker> commands = new HashMap<>();

    public FabricCommandRegistry(ArgParserRegistry parserRegistry, PluginEnvironment environment) {
        this.parserRegistry = parserRegistry;
        this.environment = environment;
    }

    @Override
    public void register(Object command) {
        Command cmdAnnotation = command.getClass().getAnnotation(Command.class);
        if (cmdAnnotation == null) {
            Axiom.logger().warn("Command {} does not have @Command annotation, skipping registration",
                command.getClass().getName());
            return;
        }

        try {
            CommandInvoker invoker = new CommandInvoker(command, parserRegistry, environment);
            commands.put(cmdAnnotation.name(), invoker);
            Axiom.logger().info("Registered command: {} {}", cmdAnnotation.name(),
                cmdAnnotation.aliases().length > 0 ? "with aliases: " + String.join(", ", cmdAnnotation.aliases()) : "");
        } catch (Exception e) {
            Axiom.logger().error("Failed to register command {}", command.getClass().getName(), e);
        }
    }

    @Override
    public void unregister(Object command) {
        Command cmdAnnotation = command.getClass().getAnnotation(Command.class);
        if (cmdAnnotation != null) {
            unregister(cmdAnnotation.name());
        }
    }

    @Override
    public void unregister(String name) {
        commands.remove(name);
        Axiom.logger().info("Unregistered command: {}", name);
    }

    public CommandInvoker get(String name) {
        return commands.get(name);
    }

    public Map<String, CommandInvoker> getAll() {
        return new HashMap<>(commands);
    }
}
