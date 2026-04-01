package com.axiommc.fabric.command;

import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.invoker.CommandInvoker;
import com.axiommc.api.command.parser.ArgParserRegistry;
import com.axiommc.api.command.parser.DoubleArgParser;
import com.axiommc.api.command.parser.FloatArgParser;
import com.axiommc.api.command.parser.IntArgParser;
import com.axiommc.api.command.parser.LongArgParser;
import com.axiommc.api.command.parser.StringArgParser;

import com.axiommc.api.event.EventBus;
import com.axiommc.api.player.Player;
import com.axiommc.api.plugin.PluginEnvironment;
import com.axiommc.api.world.World;
import com.axiommc.fabric.command.parser.FabricPlayerArgParser;
import com.axiommc.fabric.command.parser.FabricWorldArgParser;
import com.axiommc.fabric.command.parser.Vector3ArgParser;
import com.axiommc.api.math.Vector3;
import com.axiommc.fabric.player.FabricPlayer;
import com.mojang.brigadier.CommandDispatcher;
import net.minecraft.commands.CommandSourceStack;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class FabricCommandHandler {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricCommandHandler.class);

    private final EventBus eventBus;
    private final FabricCommandRegistry registry;
    private final ArgParserRegistry parserRegistry;
    private final List<FabricCommandAdapter> adapters = new ArrayList<>();
    private CommandDispatcher<CommandSourceStack> dispatcher;

    public FabricCommandHandler(EventBus eventBus) {
        this.eventBus = eventBus;
        this.parserRegistry = new ArgParserRegistry();
        this.registry = new FabricCommandRegistry(parserRegistry, PluginEnvironment.SERVER);
        registerDefaultParsers();
    }

    public void registerCommand(Object command) {
        Command cmdAnnotation = command.getClass().getAnnotation(Command.class);
        if (cmdAnnotation == null) {
            LOGGER.warn("Command {} has no @Command annotation, skipping", command.getClass().getSimpleName());
            return;
        }

        registry.register(command);

        try {
            CommandInvoker invoker = registry.get(cmdAnnotation.name());
            FabricCommandAdapter adapter = new FabricCommandAdapter(cmdAnnotation.name(), invoker);
            adapters.add(adapter);

            if (dispatcher != null) {
                dispatcher.register(adapter.buildNode());
                LOGGER.info("Dynamically registered Brigadier command: {}", cmdAnnotation.name());
            }
        } catch (Exception e) {
            LOGGER.error("Failed to register command: {}", cmdAnnotation.name(), e);
        }
    }

    public void register(Object dispatcherObj) {
        if (!(dispatcherObj instanceof CommandDispatcher)) {
            LOGGER.warn("Expected CommandDispatcher, got {}", dispatcherObj.getClass().getName());
            return;
        }

        @SuppressWarnings("unchecked")
        CommandDispatcher<CommandSourceStack> brigadierDispatcher =
            (CommandDispatcher<CommandSourceStack>) dispatcherObj;

        this.dispatcher = brigadierDispatcher;

        Map<String, CommandInvoker> commands = registry.getAll();
        for (Map.Entry<String, CommandInvoker> entry : commands.entrySet()) {
            try {
                CommandInvoker invoker = entry.getValue();
                FabricCommandAdapter adapter = new FabricCommandAdapter(entry.getKey(), invoker);
                adapters.add(adapter);
                dispatcher.register(adapter.buildNode());
                LOGGER.debug("Registered Brigadier command: {}", entry.getKey());
            } catch (Exception e) {
                LOGGER.error("Failed to register command: {}", entry.getKey(), e);
            }
        }
    }

    private void registerDefaultParsers() {
        parserRegistry.register(Integer.class, new IntArgParser());
        parserRegistry.register(Long.class, new LongArgParser());
        parserRegistry.register(Float.class, new FloatArgParser());
        parserRegistry.register(Double.class, new DoubleArgParser());
        parserRegistry.register(String.class, new StringArgParser());
        parserRegistry.register(Player.class, new FabricPlayerArgParser());
        parserRegistry.register(World.class, new FabricWorldArgParser());
        parserRegistry.register(Vector3.class, new Vector3ArgParser());
    }
}
