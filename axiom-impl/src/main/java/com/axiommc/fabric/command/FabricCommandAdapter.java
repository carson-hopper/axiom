package com.axiommc.fabric.command;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.invoker.CommandInvoker;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.event.adapter.CommandExecuteAdapter;
import com.mojang.brigadier.Command;
import com.mojang.brigadier.arguments.StringArgumentType;
import com.mojang.brigadier.builder.LiteralArgumentBuilder;
import com.mojang.brigadier.builder.RequiredArgumentBuilder;
import com.mojang.brigadier.context.CommandContext;
import com.mojang.brigadier.suggestion.Suggestions;
import com.mojang.brigadier.suggestion.SuggestionsBuilder;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import net.minecraft.commands.CommandSourceStack;

public record FabricCommandAdapter(String commandName, CommandInvoker invoker) {

    public LiteralArgumentBuilder<CommandSourceStack> buildNode() {
        LiteralArgumentBuilder<CommandSourceStack> builder =
            LiteralArgumentBuilder.literal(commandName);

        RequiredArgumentBuilder<CommandSourceStack, String> argsBuilder =
            RequiredArgumentBuilder.<CommandSourceStack, String>argument(
                    "args", StringArgumentType.greedyString())
                .suggests(this::suggest)
                .executes(this::executeWithArgs);

        return builder.then(argsBuilder).executes(this::executeNoArgs);
    }

    private int executeNoArgs(CommandContext<CommandSourceStack> ctx) {
        try {
            FabricCommandSender sender = new FabricCommandSender(ctx.getSource());
            if (!CommandExecuteAdapter.firePreEvent(sender, commandName)) {
                return Command.SINGLE_SUCCESS;
            }
            invoker.execute(sender, new String[0]);
            CommandExecuteAdapter.firePostEvent(sender, commandName);
        } catch (Exception e) {
            Axiom.logger().error("Error executing command: /{}", commandName, e);
        }
        return Command.SINGLE_SUCCESS;
    }

    private int executeWithArgs(CommandContext<CommandSourceStack> ctx) {
        try {
            String rawArgs = StringArgumentType.getString(ctx, "args");
            String[] args =
                rawArgs.trim().isEmpty() ? new String[0] : rawArgs.trim().split("\\s+");
            FabricCommandSender sender = new FabricCommandSender(ctx.getSource());
            String fullCommand = commandName + (rawArgs.trim().isEmpty() ? "" : " " + rawArgs);
            if (!CommandExecuteAdapter.firePreEvent(sender, fullCommand)) {
                return Command.SINGLE_SUCCESS;
            }
            invoker.execute(sender, args);
            CommandExecuteAdapter.firePostEvent(sender, fullCommand);
        } catch (Exception e) {
            Axiom.logger().error("Error executing command: /{}", commandName, e);
        }
        return Command.SINGLE_SUCCESS;
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
