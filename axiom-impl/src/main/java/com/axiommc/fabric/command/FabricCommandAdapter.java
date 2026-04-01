package com.axiommc.fabric.command;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.invoker.CommandInvoker;
import com.axiommc.fabric.console.ConsoleHistory;
import com.axiommc.fabric.event.adapter.CommandExecuteAdapter;
import com.mojang.brigadier.Command;
import com.mojang.brigadier.arguments.StringArgumentType;
import com.mojang.brigadier.builder.LiteralArgumentBuilder;
import com.mojang.brigadier.builder.RequiredArgumentBuilder;
import com.mojang.brigadier.context.CommandContext;
import com.mojang.brigadier.suggestion.Suggestions;
import com.mojang.brigadier.suggestion.SuggestionsBuilder;
import net.minecraft.commands.CommandSourceStack;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;
import java.util.concurrent.CompletableFuture;

public record FabricCommandAdapter(String commandName, CommandInvoker invoker) {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricCommandAdapter.class);

    public LiteralArgumentBuilder<CommandSourceStack> buildNode() {
        LiteralArgumentBuilder<CommandSourceStack> builder = LiteralArgumentBuilder.literal(commandName);

        RequiredArgumentBuilder<CommandSourceStack, String> argsBuilder =
                RequiredArgumentBuilder.<CommandSourceStack, String>argument("args", StringArgumentType.greedyString())
                        .suggests(this::suggest)
                        .executes(this::executeWithArgs);

        return builder.then(argsBuilder).executes(this::executeNoArgs);
    }

    private int executeNoArgs(CommandContext<CommandSourceStack> ctx) {
        try {
            FabricCommandSender sender = new FabricCommandSender(ctx.getSource());
            if (!CommandExecuteAdapter.fireEvent(sender, commandName)) {
                return Command.SINGLE_SUCCESS;
            }
            invoker.execute(sender, new String[0]);
            addToConsoleHistory(ctx.getSource(), "");
        } catch (Exception e) {
            LOGGER.error("Error executing command: /{}", commandName, e);
        }
        return Command.SINGLE_SUCCESS;
    }

    private int executeWithArgs(CommandContext<CommandSourceStack> ctx) {
        try {
            String rawArgs = StringArgumentType.getString(ctx, "args");
            String[] args = rawArgs.trim().isEmpty() ? new String[0] : rawArgs.trim().split("\\s+");
            FabricCommandSender sender = new FabricCommandSender(ctx.getSource());
            String fullCommand = commandName + (rawArgs.trim().isEmpty() ? "" : " " + rawArgs);
            if (!CommandExecuteAdapter.fireEvent(sender, fullCommand)) {
                return Command.SINGLE_SUCCESS;
            }
            invoker.execute(sender, args);
            addToConsoleHistory(ctx.getSource(), rawArgs);
        } catch (Exception e) {
            LOGGER.error("Error executing command: /{}", commandName, e);
        }
        return Command.SINGLE_SUCCESS;
    }

    private void addToConsoleHistory(CommandSourceStack source, String args) {
        if (source.getEntity() == null) {
            String fullCommand = commandName + (args.isEmpty() ? "" : " " + args);
            ConsoleHistory.addCommand(fullCommand);
        }
    }

    private CompletableFuture<Suggestions> suggest(
            CommandContext<CommandSourceStack> ctx, SuggestionsBuilder builder) {
        CommandSender sender = new FabricCommandSender(ctx.getSource());
        String input = builder.getRemaining();
        String[] args = input.isEmpty() ? new String[0] : input.split("\\s+", -1);
        List<String> suggestions = invoker.suggest(sender, args);

        String prefix = input.contains(" ") ? input.substring(input.lastIndexOf(' ') + 1) : input;
        for (String s : suggestions) {
            if (s.toLowerCase().startsWith(prefix.toLowerCase())) {
                builder.suggest(s);
            }
        }

        return builder.buildFuture();
    }
}
