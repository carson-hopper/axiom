package com.axiommc.fabric.command;

import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.invoker.CommandInvoker;
import com.axiommc.api.command.parser.ArgParserRegistry;
import com.axiommc.api.command.parser.DoubleArgParser;
import com.axiommc.api.command.parser.FloatArgParser;
import com.axiommc.api.command.parser.IntArgParser;
import com.axiommc.api.command.parser.LongArgParser;
import com.axiommc.api.command.parser.StringArgParser;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.plugin.PluginEnvironment;
import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.builder.LiteralArgumentBuilder;
import net.minecraft.commands.CommandSourceStack;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Map;

public class FabricCommandHandler {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricCommandHandler.class);

    private final EventBus eventBus;
    private final FabricCommandRegistry registry;
    private final ArgParserRegistry parserRegistry;
    private CommandDispatcher<CommandSourceStack> dispatcher;

    public FabricCommandHandler(EventBus eventBus) {
        this.eventBus = eventBus;
        this.parserRegistry = new ArgParserRegistry();
        this.registry = new FabricCommandRegistry(parserRegistry, PluginEnvironment.SERVER);

        registerDefaultParsers();
    }

    public void registerCommand(com.axiommc.api.command.Command command) {
        registry.register(command);

        // If dispatcher is available (after CommandRegistrationCallback), register with Brigadier immediately
        if (dispatcher != null) {
            CommandMeta meta = command.getClass().getAnnotation(CommandMeta.class);
            if (meta != null) {
                CommandInvoker invoker = registry.get(meta.name());
                if (invoker != null) {
                    try {
                        registerBrigadierCommand(dispatcher, meta.name(), invoker);
                        LOGGER.info("Dynamically registered Brigadier command: {}", meta.name());
                    } catch (Exception e) {
                        LOGGER.error("Failed to dynamically register Brigadier command: {}", meta.name(), e);
                    }
                }
            }
        }
    }

    public void register(Object dispatcher) {
        if (!(dispatcher instanceof CommandDispatcher)) {
            LOGGER.warn("Expected CommandDispatcher, got {}", dispatcher.getClass().getName());
            return;
        }

        @SuppressWarnings("unchecked")
        CommandDispatcher<CommandSourceStack> brigadierDispatcher =
            (CommandDispatcher<CommandSourceStack>) dispatcher;

        this.dispatcher = brigadierDispatcher;

        Map<String, CommandInvoker> commands = registry.getAll();
        for (Map.Entry<String, CommandInvoker> entry : commands.entrySet()) {
            String commandName = entry.getKey();
            CommandInvoker invoker = entry.getValue();

            try {
                registerBrigadierCommand(brigadierDispatcher, commandName, invoker);
                LOGGER.debug("Registered Brigadier command: {}", commandName);
            } catch (Exception e) {
                LOGGER.error("Failed to register Brigadier command: {}", commandName, e);
            }
        }
    }

    private void registerBrigadierCommand(CommandDispatcher<CommandSourceStack> dispatcher,
                                         String commandName, CommandInvoker invoker) {
        LiteralArgumentBuilder<CommandSourceStack> builder =
            LiteralArgumentBuilder.literal(commandName);

        builder.executes(context -> {
            CommandSourceStack source = context.getSource();
            String[] args = {};
            FabricCommandSender sender = new FabricCommandSender(source);
            invoker.execute(sender, args);
            return 1;
        });

        dispatcher.register(builder);
    }

    private void registerDefaultParsers() {
        parserRegistry.register(String.class, new StringArgParser());
        parserRegistry.register(int.class, new IntArgParser());
        parserRegistry.register(Integer.class, new IntArgParser());
        parserRegistry.register(long.class, new LongArgParser());
        parserRegistry.register(Long.class, new LongArgParser());
        parserRegistry.register(float.class, new FloatArgParser());
        parserRegistry.register(Float.class, new FloatArgParser());
        parserRegistry.register(double.class, new DoubleArgParser());
        parserRegistry.register(Double.class, new DoubleArgParser());
    }

    public FabricCommandRegistry getRegistry() {
        return registry;
    }
}
