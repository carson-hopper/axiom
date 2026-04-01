package com.axiommc.api.command.invoker;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.CommandSide;
import com.axiommc.api.command.SenderType;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Default;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Flag;
import com.axiommc.api.command.annotation.Greedy;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Range;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.command.annotation.TabComplete;
import com.axiommc.api.command.annotation.Usage;
import com.axiommc.api.command.parser.ArgParseException;
import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.api.command.parser.ArgParserRegistry;
import com.axiommc.api.entity.Entity;
import com.axiommc.api.plugin.PluginEnvironment;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Invokes command methods with reflection, parsing arguments and handling subcommands.
 *
 * <p>Uses string-based class name matching for parameter type checks (e.g., checking
 * if a parameter is CommandSender by comparing fully-qualified class name) to handle
 * plugin classloader isolation. Plugins may be loaded in separate URLClassLoaders,
 * which means CommandSender.class from the plugin's classloader differs from the
 * framework's CommandSender.class. String-based matching works across classloader
 * boundaries and is the most reliable approach here.
 */
public class CommandInvoker {

    private static final Logger LOGGER = LoggerFactory.getLogger(CommandInvoker.class);
    private static final String COMMAND_SENDER_CLASS_NAME = "com.axiommc.api.command.CommandSender";

    private final Object command;
    private final ArgParserRegistry parserRegistry;
    private final PluginEnvironment environment;
    private final List<Method> executeMethods = new ArrayList<>();
    private final Map<String, Method> subcommandMethods = new LinkedHashMap<>();
    /** Subcommands that exist but were excluded because their side doesn't match this environment. */
    private final Set<String> excludedSubcommands = new LinkedHashSet<>();

    public CommandInvoker(Object command, ArgParserRegistry parserRegistry, PluginEnvironment environment) {
        this.command = command;
        this.parserRegistry = parserRegistry;
        this.environment = environment;
        scan();
    }

    private boolean sideMatches(CommandSide required) {
        if (required == CommandSide.BOTH) return true;
        if (required == CommandSide.SERVER) return environment == PluginEnvironment.SERVER;
        if (required == CommandSide.PROXY)  return environment == PluginEnvironment.PROXY;
        return true;
    }

    private void scan() {
        for (Method method : command.getClass().getMethods()) {
            if (method.isAnnotationPresent(Execute.class)) {
                executeMethods.add(method);
            } else if (method.isAnnotationPresent(Subcommand.class)) {
                Subcommand sub = method.getAnnotation(Subcommand.class);
                String name = sub.value().isEmpty() ? method.getName() : sub.value();
                name = name.toLowerCase();
                if (!sideMatches(sub.side())) {
                    excludedSubcommands.add(name);
                    continue;
                }
                subcommandMethods.put(name, method);
            }
        }

        // Validate @Greedy and @Flag parameters
        List<Method> allMethods = new ArrayList<>(subcommandMethods.values());
        allMethods.addAll(executeMethods);
        for (Method method : allMethods) {
            Parameter[] params = method.getParameters();
            List<Parameter> nonSenderParams = new ArrayList<>();
            for (Parameter param : params) {
                // Check by class name to handle classloader differences
                if (!isCommandSenderParameter(param)) {
                    nonSenderParams.add(param);
                }
            }

            // Build list of positional parameters (excluding flags) for @Greedy validation
            List<Parameter> positionalParams = new ArrayList<>();
            for (Parameter param : nonSenderParams) {
                if (!param.isAnnotationPresent(Flag.class)) {
                    positionalParams.add(param);
                }
            }

            for (int i = 0; i < positionalParams.size(); i++) {
                Parameter param = positionalParams.get(i);
                if (param.isAnnotationPresent(Greedy.class)) {
                    if (param.getType() != String.class) {
                        throw new IllegalStateException(
                            "@Greedy can only be applied to String parameters in method: " + method.getName());
                    }
                    if (i != positionalParams.size() - 1) {
                        throw new IllegalStateException(
                            "@Greedy parameter must be the last positional parameter in method: " + method.getName());
                    }
                }
            }

            for (int i = 0; i < nonSenderParams.size(); i++) {
                Parameter param = nonSenderParams.get(i);
                if (param.isAnnotationPresent(Flag.class)) {
                    if (param.isAnnotationPresent(Greedy.class)) {
                        throw new IllegalStateException(
                            "@Flag cannot be combined with @Greedy in method: " + method.getName());
                    }
                    boolean hasDefault = param.isAnnotationPresent(Default.class);
                    boolean isOptional = param.isAnnotationPresent(com.axiommc.api.command.annotation.Optional.class);
                    if (!hasDefault && !isOptional) {
                        throw new IllegalStateException(
                            "@Flag parameter must be @Optional or have @Default in method: " + method.getName());
                    }
                }
            }
        }
    }

    public boolean execute(CommandSender sender, String[] args) {
        Class<?> commandClass = command.getClass();
        Command cmdAnnotation = commandClass.getAnnotation(Command.class);

        String commandName = cmdAnnotation != null ? cmdAnnotation.name() : commandClass.getSimpleName();
        String senderName = sender.isPlayer() ? sender.asPlayer().map(Entity::name).orElse("Unknown") : "Console";
        String argsStr = args.length > 0 ? String.join(" ", args) : "";
        LOGGER.info("Command executed: /{} {} (by: {})", commandName, argsStr, senderName);

        Method method;
        String[] methodArgs;

        if (args.length > 0 && subcommandMethods.containsKey(args[0].toLowerCase())) {
            method = subcommandMethods.get(args[0].toLowerCase());
            methodArgs = Arrays.copyOfRange(args, 1, args.length);
        } else if (args.length > 0 && excludedSubcommands.contains(args[0].toLowerCase())) {
            String target = environment == PluginEnvironment.PROXY ? "the game server" : "the proxy";
            sender.sendMessage("This subcommand is only available on " + target + ".");
            return true;
        } else if (!executeMethods.isEmpty()) {
            // Select best matching @Execute method based on argument count
            // For ambiguous cases (multiple methods with same param count), try parsing with each
            method = selectExecuteMethodWithParsing(sender, args);
            if (method == null) {
                sender.sendMessage("Unknown command.");
                return true;
            }
            methodArgs = args;
        } else {
            sender.sendMessage("Unknown command.");
            return true;
        }

        // Check permission on the selected method (method-level overrides class-level)
        String requiredPermission = getRequiredPermission(method);
        if (requiredPermission != null && !requiredPermission.isEmpty() && !sender.hasPermission(requiredPermission)) {
            sender.sendMessage("You don't have permission to use this command.");
            return true;
        }

        try {
            Object[] invokeArgs = buildArgs(sender, method, methodArgs);
            if (invokeArgs == null) return true;
            method.invoke(command, invokeArgs);
        } catch (ArgParseException e) {
            sender.sendMessage("Invalid argument: " + e.getMessage());
        } catch (InvocationTargetException e) {
            // Unwrap the underlying exception thrown by the command method
            Throwable cause = e.getCause();
            if (cause instanceof ArgParseException) {
                sender.sendMessage("Invalid argument: " + cause.getMessage());
            } else if (cause instanceof RuntimeException) {
                LOGGER.error("Error executing command: /{}", commandName, cause);
                throw (RuntimeException) cause;
            } else if (cause != null) {
                LOGGER.error("Error executing command: /{}", commandName, cause);
                throw new RuntimeException("Command execution failed: " + cause.getMessage(), cause);
            } else {
                LOGGER.error("Error executing command: /{}", commandName, e);
                throw new RuntimeException("Command execution failed", e);
            }
        } catch (IllegalAccessException e) {
            LOGGER.error("Error executing command: /{}", commandName, e);
            throw new RuntimeException("Command method is not accessible", e);
        }
        return true;
    }

    private Object[] buildArgs(CommandSender sender, Method method, String[] args) throws ArgParseException {
        Parameter[] params = method.getParameters();
        Object[] result = new Object[params.length];

        // Extract flags from args (--flagName value)
        Map<String, String> flagValues = new LinkedHashMap<>();
        List<String> positionalArgs = new ArrayList<>();

        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg.startsWith("--")) {
                String flagName = arg.substring(2);
                if (i + 1 < args.length && !args[i + 1].startsWith("--")) {
                    flagValues.put(flagName, args[i + 1]);
                    i++; // Skip the value
                } else {
                    throw new ArgParseException("Flag --" + flagName + " requires a value");
                }
            } else {
                positionalArgs.add(arg);
            }
        }

        int argIdx = 0;

        for (int i = 0; i < params.length; i++) {
            Parameter param = params[i];

            // Check by class name to handle classloader differences
            if (isCommandSenderParameter(param)) {
                result[i] = sender;
                continue;
            }

            // Handle @Flag parameters
            if (param.isAnnotationPresent(Flag.class)) {
                Flag flagAnnotation = param.getAnnotation(Flag.class);
                String flagName = flagAnnotation.value();
                String flagValue = flagValues.get(flagName);

                if (flagValue != null) {
                    ArgParser<?> parser = parserRegistry.get(param.getType());
                    if (parser == null) {
                        throw new ArgParseException("No parser for flag type: " + param.getType().getSimpleName());
                    }
                    Object parsed = parser.parse(flagValue);
                    Range range = param.getAnnotation(Range.class);
                    if (range != null && parsed instanceof Number num) {
                        double val = num.doubleValue();
                        if (val < range.min() || val > range.max()) {
                            throw new ArgParseException("Flag --" + flagName + " value out of range [" + range.min() + ", " + range.max() + "]");
                        }
                    }
                    result[i] = parsed;
                } else {
                    // Use @Default if available, otherwise use primitive default for optional flags
                    Default def = param.getAnnotation(Default.class);
                    if (def != null) {
                        ArgParser<?> parser = parserRegistry.get(param.getType());
                        if (parser != null) {
                            result[i] = parser.parse(def.value());
                        } else {
                            result[i] = null;
                        }
                    } else {
                        // Flag is @Optional without @Default - use primitive default
                        result[i] = getPrimitiveDefault(param.getType());
                    }
                }
                continue;
            }

            // @Default alone implies optionality — no @Optional annotation required
            boolean hasDefault = param.isAnnotationPresent(Default.class);
            boolean isOptional = param.isAnnotationPresent(com.axiommc.api.command.annotation.Optional.class) || hasDefault;
            boolean isGreedy = param.isAnnotationPresent(Greedy.class);

            if (isGreedy && param.getType() == String.class) {
                if (argIdx >= positionalArgs.size()) {
                    if (isOptional) {
                        Default def = param.getAnnotation(Default.class);
                        result[i] = def != null ? def.value() : null;
                    } else {
                        Arg arg = param.getAnnotation(Arg.class);
                        String name = arg != null ? arg.value() : param.getName();
                        sender.sendMessage("Missing required argument: <" + name + ">");
                        return null;
                    }
                } else {
                    result[i] = String.join(" ", positionalArgs.subList(argIdx, positionalArgs.size()));
                    argIdx = positionalArgs.size();
                }
                continue;
            }

            if (argIdx >= positionalArgs.size()) {
                if (isOptional) {
                    Default def = param.getAnnotation(Default.class);
                    if (def != null) {
                        ArgParser<?> parser = parserRegistry.get(param.getType());
                        if (parser != null) {
                            Object parsed = parser.parse(def.value());
                            // Apply @Range check to default value too
                            Range range = param.getAnnotation(Range.class);
                            if (range != null && parsed instanceof Number num) {
                                double val = num.doubleValue();
                                if (val < range.min() || val > range.max()) {
                                    throw new ArgParseException("Default value " + def.value() + " is out of range [" + range.min() + ", " + range.max() + "]");
                                }
                            }
                            result[i] = parsed;
                        } else {
                            result[i] = null;
                        }
                    } else {
                        result[i] = null;
                    }
                } else {
                    Arg arg = param.getAnnotation(Arg.class);
                    String name = arg != null ? arg.value() : param.getName();
                    sender.sendMessage("Missing required argument: <" + name + ">");
                    return null;
                }
                continue;
            }

            String raw = positionalArgs.get(argIdx++);
            ArgParser<?> parser = parserRegistry.get(param.getType());
            if (parser == null) {
                throw new ArgParseException("No parser for type: " + param.getType().getSimpleName());
            }

            Object parsed = parser.parse(raw);

            Range range = param.getAnnotation(Range.class);
            if (range != null && parsed instanceof Number num) {
                double val = num.doubleValue();
                if (val < range.min() || val > range.max()) {
                    throw new ArgParseException("Value " + raw + " is out of range [" + range.min() + ", " + range.max() + "]");
                }
            }

            result[i] = parsed;
        }
        return result;
    }

    public List<String> suggest(CommandSender sender, String[] args) {

        if (args.length == 0) {
            // When no args, suggest subcommands or first parameter of @Execute methods
            Set<String> seen = new LinkedHashSet<>(subcommandMethods.keySet());
            if (!executeMethods.isEmpty()) {
                // Get suggestions from methods that can accept 1 argument
                List<Method> methods = getMatchingExecuteMethods(1);
                for (Method method : methods) {
                    seen.addAll(getParamSuggestions(method, 0, ""));
                }
            }

            // Only add class-level usage if exactly one total usage exists
            List<String> allUsages = collectAllUsages();
            if (allUsages.size() == 1) {
                seen.add(allUsages.get(0));
            }

            return new ArrayList<>(seen);
        }

        String last = args[args.length - 1].toLowerCase();

        if (args.length == 1) {
            Set<String> seen = new LinkedHashSet<>(subcommandMethods.keySet());
            if (!executeMethods.isEmpty()) {
                // Get suggestions from ALL matching @Execute methods
                List<Method> methods = getMatchingExecuteMethods(args.length);
                for (Method method : methods) {
                    seen.addAll(getParamSuggestions(method, 0, last));
                }
            }

            // Only add usages if exactly one total usage exists
            List<String> allUsages = collectAllUsages();
            if (allUsages.size() == 1) {
                seen.add(allUsages.get(0));
            }

            return filterPrefix(new ArrayList<>(seen), last);
        }

        String sub = args[0].toLowerCase();
        if (subcommandMethods.containsKey(sub)) {
            Method method = subcommandMethods.get(sub);
            int argPos = args.length - 2; // args[0] is subcommand name, args[1..] are the actual args
            return filterPrefix(getParamSuggestions(method, argPos, last), last);
        }

        if (!executeMethods.isEmpty()) {
            // Get suggestions from ALL matching @Execute methods
            // We have 'args.length' arguments total and want suggestions for the last one
            // So we need methods that can handle 'args.length' arguments
            List<Method> methods = getMatchingExecuteMethods(args.length);
            Set<String> seen = new LinkedHashSet<>();
            for (Method method : methods) {
                // For methods with @Greedy, pass all remaining args as one partial string
                if (hasGreedyParameter(method)) {
                    // Find the greedy parameter position
                    int greedyParamPos = findGreedyParameterIndex(method);
                    if (greedyParamPos >= 0) {
                        // Reconstruct the entire greedy input by joining all args from that position
                        String greedyInput = String.join(" ", Arrays.copyOfRange(args, greedyParamPos, args.length));
                        seen.addAll(getParamSuggestions(method, greedyParamPos, greedyInput));
                    }
                } else {
                    int argPos = args.length - 1;  // Position of the last argument (0-indexed)
                    seen.addAll(getParamSuggestions(method, argPos, last));
                }
            }
            return filterPrefix(new ArrayList<>(seen), last);
        }

        return Collections.emptyList();
    }

    /**
     * Get all @Execute methods that can handle the given argument count.
     * Returns exact matches first, then methods with optional parameters.
     * Methods with @Greedy parameters can accept unlimited arguments.
     */
    private List<Method> getMatchingExecuteMethods(int argCount) {
        List<Method> matching = new ArrayList<>();

        // First pass: find exact matches
        for (Method m : executeMethods) {
            int paramCount = countNonSenderParams(m);
            if (paramCount == argCount) {
                matching.add(m);
            }
        }

        // If exact matches found, return only those
        if (!matching.isEmpty()) {
            return matching;
        }

        // Second pass: find methods that can handle with optional params
        for (Method m : executeMethods) {
            int minRequired = countRequiredParams(m);
            int maxAccepted = countNonSenderParams(m);

            // If method has @Greedy parameter, it can accept unlimited arguments
            if (hasGreedyParameter(m)) {
                maxAccepted = Integer.MAX_VALUE;
            }

            if (argCount >= minRequired && argCount <= maxAccepted) {
                matching.add(m);
                LOGGER.debug("Optional match:  {}({}-{})", m.getName(), minRequired, maxAccepted);
            }
        }

        // If no methods found but we have executeMethods, return all as fallback
        if (matching.isEmpty() && !executeMethods.isEmpty()) {
            matching.addAll(executeMethods);
            LOGGER.debug("No matches found, using all methods as fallback");
        }

        return matching;
    }

    /** Check if method has a @Greedy parameter */
    private boolean hasGreedyParameter(Method method) {
        for (Parameter param : method.getParameters()) {
            if (param.isAnnotationPresent(Greedy.class)) {
                return true;
            }
        }
        return false;
    }

    /** Find the positional index of the @Greedy parameter, or -1 if none exists */
    private int findGreedyParameterIndex(Method method) {
        int paramIndex = 0;
        for (Parameter param : method.getParameters()) {
            // Skip CommandSender and @Flag parameters
            String className = param.getType().getName();
            boolean isSender = className.equals("com.axiommc.api.command.CommandSender");
            boolean isFlag = param.isAnnotationPresent(Flag.class);
            if (isSender || isFlag) continue;

            if (param.isAnnotationPresent(Greedy.class)) {
                return paramIndex;
            }
            paramIndex++;
        }
        return -1;
    }

    /**
     * Select the best @Execute method by trying to parse arguments.
     * For each candidate method (by param count), attempt to build args.
     * Returns the first method where all arguments parse successfully.
     */
    private boolean hasMethodsForType(SenderType type) {
        for (Method m : executeMethods) {
            Execute execute = m.getAnnotation(Execute.class);
            if (execute != null && execute.type() == type) {
                return true;
            }
        }
        return false;
    }

    private boolean senderTypeMatches(Method method, CommandSender sender) {
        Execute execute = method.getAnnotation(Execute.class);
        if (execute == null) {
            return true;
        }
        SenderType type = execute.type();
        if (type == SenderType.BOTH) {
            return true;
        }
        if (type == SenderType.PLAYER) {
            return sender.isPlayer();
        }
        if (type == SenderType.CONSOLE) {
            return !sender.isPlayer();
        }
        return true;
    }

    private Method selectExecuteMethodWithParsing(CommandSender sender, String[] args) {
        List<Method> candidates = getMatchingExecuteMethods(args.length);

        // Filter by sender type — prefer specific matches over BOTH
        List<Method> specificMatches = new ArrayList<>();
        List<Method> bothMatches = new ArrayList<>();
        for (Method m : candidates) {
            if (!senderTypeMatches(m, sender)) {
                continue;
            }
            Execute execute = m.getAnnotation(Execute.class);
            SenderType type = execute != null ? execute.type() : SenderType.BOTH;
            if (type == SenderType.BOTH) {
                bothMatches.add(m);
            } else {
                specificMatches.add(m);
            }
        }

        // Use specific matches first, fall back to BOTH
        List<Method> filtered = specificMatches.isEmpty() ? bothMatches : specificMatches;

        if (filtered.isEmpty()) {
            // No matching methods — send a default message based on what's available
            if (sender.isPlayer() && hasMethodsForType(SenderType.CONSOLE)) {
                sender.sendMessage("This command can only be run from the console.");
            } else if (!sender.isPlayer() && hasMethodsForType(SenderType.PLAYER)) {
                sender.sendMessage("This command can only be run by a player.");
            }
            return null;
        }

        if (filtered.size() == 1) {
            return filtered.getFirst();
        }

        // Multiple candidates: try parsing with each and use first that succeeds
        for (Method candidate : filtered) {
            try {
                Object[] invokeArgs = buildArgs(sender, candidate, args);
                if (invokeArgs != null) {
                    return candidate;
                }
            } catch (Exception e) {
                continue;
            }
        }

        return filtered.getFirst();
    }

    /** Count parameters that are not CommandSender and not @Flag */
    private int countNonSenderParams(Method method) {
        int count = 0;
        for (Parameter p : method.getParameters()) {
            // Check by class name to handle classloader differences
            String className = p.getType().getName();
            boolean isSender = className.equals("com.axiommc.api.command.CommandSender");
            boolean isFlag = p.isAnnotationPresent(Flag.class);
            if (!isSender && !isFlag) {
                count++;
            }
        }
        return count;
    }

    /** Count required parameters (not marked @Optional or @Default), excluding @Flag */
    private int countRequiredParams(Method method) {
        int count = 0;
        for (Parameter p : method.getParameters()) {
            // Check by class name to handle classloader differences
            if (!p.getType().getName().equals("com.axiommc.api.command.CommandSender")) {
                // Exclude @Flag parameters
                if (!p.isAnnotationPresent(Flag.class)) {
                    if (!p.isAnnotationPresent(com.axiommc.api.command.annotation.Optional.class) && !p.isAnnotationPresent(Default.class)) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    /** Get default value for primitive types (0 for int, false for boolean, etc.) */
    private Object getPrimitiveDefault(Class<?> type) {
        if (type == int.class) return 0;
        if (type == long.class) return 0L;
        if (type == short.class) return (short) 0;
        if (type == byte.class) return (byte) 0;
        if (type == float.class) return 0.0f;
        if (type == double.class) return 0.0;
        if (type == boolean.class) return false;
        if (type == char.class) return '\0';
        return null;
    }

    private List<String> getParamSuggestions(Method method, int argPos, String partial) {
        Parameter[] params = method.getParameters();
        int commandParamIndex = 0;
        for (Parameter param : params) {
            // Check by class name to handle classloader differences
            if (isCommandSenderParameter(param)) continue;

            // Skip @Flag parameters in positional counting
            if (param.isAnnotationPresent(Flag.class)) continue;

            if (commandParamIndex == argPos) {
                // Check for dynamic tab completion first
                DynamicTabComplete dtc = param.getAnnotation(DynamicTabComplete.class);
                if (dtc != null) {
                    try {
                        Method suggestionMethod = command.getClass().getMethod(dtc.value(), String.class);
                        Object result = suggestionMethod.invoke(command, partial);
                        if (result instanceof List<?>) {
                            @SuppressWarnings("unchecked")
                            List<String> suggestions = (List<String>) result;
                            return suggestions;
                        }
                    } catch (Exception e) {
                        // Fall through to other suggestion methods
                    }
                }

                // Check for static tab complete annotation
                TabComplete tc = param.getAnnotation(TabComplete.class);
                if (tc != null) {
                    return Arrays.asList(tc.value());
                }

                // Check for arg parser
                ArgParser<?> parser = parserRegistry.get(param.getType());
                if (parser != null) {
                    List<String> suggestions = parser.suggest(partial);
                    // If parser has no suggestions, use the parameter name as guidance
                    if (suggestions.isEmpty()) {
                        Arg arg = param.getAnnotation(Arg.class);
                        String paramName = arg != null ? arg.value() : param.getName();
                        return Arrays.asList("<" + paramName + ">");
                    }
                    return suggestions;
                }
                return Collections.emptyList();
            }
            commandParamIndex++;
        }
        return Collections.emptyList();
    }

    /**
     * Gets the required permission for a method.
     *
     * <p>Checks method-level @Permission first (overrides class-level),
     * then falls back to class-level @Permission, then returns null if neither exists.
     */
    private String getRequiredPermission(Method method) {
        // Check method-level permission first (overrides)
        Permission methodPermission = method.getAnnotation(Permission.class);
        if (methodPermission != null) {
            return methodPermission.value();
        }

        // Fall back to class-level permission
        Class<?> commandClass = command.getClass();
        Permission classPermission = commandClass.getAnnotation(Permission.class);
        if (classPermission != null) {
            return classPermission.value();
        }

        return null;
    }

    /**
     * Collects all unique usage strings from class-level and method-level @Usage annotations.
     */
    private List<String> collectAllUsages() {
        List<String> usages = new ArrayList<>();

        // Collect from class-level
        Usage classUsage = command.getClass().getAnnotation(Usage.class);
        if (classUsage != null && !classUsage.value().isEmpty()) {
            usages.add(classUsage.value());
        }

        // Collect from @Execute methods
        for (Method method : executeMethods) {
            Usage methodUsage = method.getAnnotation(Usage.class);
            if (methodUsage != null && !methodUsage.value().isEmpty()) {
                usages.add(methodUsage.value());
            }
        }

        // Collect from @Subcommand methods
        for (Method method : subcommandMethods.values()) {
            Usage methodUsage = method.getAnnotation(Usage.class);
            if (methodUsage != null && !methodUsage.value().isEmpty()) {
                usages.add(methodUsage.value());
            }
        }

        return usages;
    }

    /**
     * Checks if a parameter's type is CommandSender by fully-qualified class name.
     *
     * <p>Uses string matching to handle classloader isolation — plugins may have
     * CommandSender loaded from a different classloader, making Class.equals() fail.
     */
    private boolean isCommandSenderParameter(Parameter param) {
        return param.getType().getName().equals(COMMAND_SENDER_CLASS_NAME);
    }

    private List<String> filterPrefix(List<String> list, String prefix) {
        if (prefix.isEmpty()) return list;
        List<String> result = new ArrayList<>();
        for (String s : list) {
            if (s.toLowerCase().startsWith(prefix)) result.add(s);
        }
        return result;
    }
}
