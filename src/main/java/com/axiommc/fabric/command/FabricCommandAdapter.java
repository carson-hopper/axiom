package com.axiommc.fabric.command;

import com.mojang.brigadier.Command;
import com.mojang.brigadier.arguments.StringArgumentType;
import com.mojang.brigadier.builder.LiteralArgumentBuilder;
import com.mojang.brigadier.builder.RequiredArgumentBuilder;
import com.mojang.brigadier.context.CommandContext;
import com.mojang.brigadier.suggestion.SuggestionsBuilder;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.invoker.CommandInvoker;
import net.minecraft.commands.CommandSourceStack;

import java.util.List;
import java.util.concurrent.CompletableFuture;

public class FabricCommandAdapter {

    private final String commandName;
    private final CommandInvoker invoker;

    public FabricCommandAdapter(String commandName, CommandInvoker invoker) {
        this.commandName = commandName;
        this.invoker = invoker;
    }

    public LiteralArgumentBuilder<CommandSourceStack> buildNode() {
        LiteralArgumentBuilder<CommandSourceStack> builder = LiteralArgumentBuilder.literal(commandName);

        RequiredArgumentBuilder<CommandSourceStack, String> argsBuilder =
            RequiredArgumentBuilder.<CommandSourceStack, String>argument("args", StringArgumentType.greedyString())
                .suggests(this::suggest)
                .executes(this::executeWithArgs);

        return builder.then(argsBuilder).executes(this::executeNoArgs);
    }

    private int executeNoArgs(CommandContext<CommandSourceStack> ctx) {
        invoker.execute(new FabricCommandSender(ctx.getSource()), new String[0]);
        return Command.SINGLE_SUCCESS;
    }

    private int executeWithArgs(CommandContext<CommandSourceStack> ctx) {
        String rawArgs = StringArgumentType.getString(ctx, "args");
        String[] args = rawArgs.trim().isEmpty() ? new String[0] : rawArgs.trim().split("\\s+");
        invoker.execute(new FabricCommandSender(ctx.getSource()), args);
        return Command.SINGLE_SUCCESS;
    }

    private CompletableFuture<com.mojang.brigadier.suggestion.Suggestions> suggest(
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

    public String getCommandName() {
        return commandName;
    }
}
