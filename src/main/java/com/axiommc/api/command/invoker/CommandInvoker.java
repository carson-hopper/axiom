package com.axiommc.api.command.invoker;

import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.CommandSide;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Default;
import com.axiommc.api.command.annotation.DynamicTabComplete;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Greedy;
import com.axiommc.api.command.annotation.Range;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.command.annotation.TabComplete;
import com.axiommc.api.command.parser.ArgParseException;
import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.api.command.parser.ArgParserRegistry;
import com.axiommc.api.plugin.PluginEnvironment;

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
import java.util.Optional;
import java.util.Set;

public class CommandInvoker {

    private final Command command;
    private final ArgParserRegistry parserRegistry;
    private final PluginEnvironment environment;
    private final List<Method> executeMethods = new ArrayList<>();
    private final Map<String, Method> subcommandMethods = new LinkedHashMap<>();
    /** Subcommands that exist but were excluded because their side doesn't match this environment. */
    private final Set<String> excludedSubcommands = new LinkedHashSet<>();

    public CommandInvoker(Command command, ArgParserRegistry parserRegistry, PluginEnvironment environment) {
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

        // Validate @Greedy parameters
        List<Method> allMethods = new ArrayList<>(subcommandMethods.values());
        allMethods.addAll(executeMethods);
        for (Method method : allMethods) {
            Parameter[] params = method.getParameters();
            List<Parameter> nonSenderParams = new ArrayList<>();
            for (Parameter param : params) {
                if (!CommandSender.class.isAssignableFrom(param.getType())) {
                    nonSenderParams.add(param);
                }
            }
            for (int i = 0; i < nonSenderParams.size(); i++) {
                Parameter param = nonSenderParams.get(i);
                if (param.isAnnotationPresent(Greedy.class)) {
                    if (param.getType() != String.class) {
                        throw new IllegalStateException(
                            "@Greedy can only be applied to String parameters in method: " + method.getName());
                    }
                    if (i != nonSenderParams.size() - 1) {
                        throw new IllegalStateException(
                            "@Greedy parameter must be the last parameter in method: " + method.getName());
                    }
                }
            }
        }
    }

    public boolean execute(CommandSender sender, String[] args) {
        CommandMeta meta = command.getClass().getAnnotation(CommandMeta.class);
        if (meta != null && !meta.permission().isEmpty() && !sender.hasPermission(meta.permission())) {
            sender.sendMessage("You don't have permission to use this command.");
            return true;
        }

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
            method = selectExecuteMethod(args.length);
            if (method == null) {
                sender.sendMessage("Unknown command.");
                return true;
            }
            methodArgs = args;
        } else {
            sender.sendMessage("Unknown command.");
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
                throw (RuntimeException) cause;
            } else if (cause != null) {
                throw new RuntimeException("Command execution failed: " + cause.getMessage(), cause);
            } else {
                throw new RuntimeException("Command execution failed", e);
            }
        } catch (IllegalAccessException e) {
            throw new RuntimeException("Command method is not accessible", e);
        }
        return true;
    }

    private Object[] buildArgs(CommandSender sender, Method method, String[] args) throws ArgParseException {
        Parameter[] params = method.getParameters();
        Object[] result = new Object[params.length];
        int argIdx = 0;

        for (int i = 0; i < params.length; i++) {
            Parameter param = params[i];

            if (CommandSender.class.isAssignableFrom(param.getType())) {
                result[i] = sender;
                continue;
            }

            // @Default alone implies optionality — no @Optional annotation required
            boolean hasDefault = param.isAnnotationPresent(Default.class);
            boolean isOptional = param.isAnnotationPresent(com.axiommc.api.command.annotation.Optional.class) || hasDefault;
            boolean isGreedy = param.isAnnotationPresent(Greedy.class);

            if (isGreedy && param.getType() == String.class) {
                if (argIdx >= args.length) {
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
                    result[i] = String.join(" ", Arrays.copyOfRange(args, argIdx, args.length));
                    argIdx = args.length;
                }
                continue;
            }

            if (argIdx >= args.length) {
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

            String raw = args[argIdx++];
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
        CommandMeta meta = command.getClass().getAnnotation(CommandMeta.class);
        if (meta != null && !meta.permission().isEmpty() && !sender.hasPermission(meta.permission())) {
            return Collections.emptyList();
        }

        if (args.length == 0) {
            // When no args, return subcommand names as suggestions
            return new ArrayList<>(subcommandMethods.keySet());
        }

        String last = args[args.length - 1].toLowerCase();

        if (args.length == 1) {
            Set<String> seen = new LinkedHashSet<>(subcommandMethods.keySet());
            if (!executeMethods.isEmpty()) {
                // Get suggestions from the first matching @Execute method
                Method method = selectExecuteMethod(args.length);
                if (method != null) {
                    seen.addAll(getParamSuggestions(method, 0, last));
                }
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
            // Get suggestions from the selected @Execute method
            Method method = selectExecuteMethod(args.length - 1);
            if (method != null) {
                int argPos = args.length - 1;
                return filterPrefix(getParamSuggestions(method, argPos, last), last);
            }
        }

        return Collections.emptyList();
    }

    /**
     * Select the best @Execute method for the given argument count.
     * Prefers exact matches, then methods with optional parameters that can handle the arg count.
     */
    private Method selectExecuteMethod(int argCount) {
        // First pass: find exact match
        for (Method m : executeMethods) {
            int paramCount = countNonSenderParams(m);
            if (paramCount == argCount) {
                return m;
            }
        }

        // Second pass: find method that can handle with optional params
        for (Method m : executeMethods) {
            int minRequired = countRequiredParams(m);
            int maxAccepted = countNonSenderParams(m);
            if (argCount >= minRequired && argCount <= maxAccepted) {
                return m;
            }
        }

        // Fallback: return first method if any exist
        return executeMethods.isEmpty() ? null : executeMethods.getFirst();
    }

    /** Count parameters that are not CommandSender */
    private int countNonSenderParams(Method method) {
        int count = 0;
        for (Parameter p : method.getParameters()) {
            if (!CommandSender.class.isAssignableFrom(p.getType())) {
                count++;
            }
        }
        return count;
    }

    /** Count required parameters (not marked @Optional or @Default) */
    private int countRequiredParams(Method method) {
        int count = 0;
        for (Parameter p : method.getParameters()) {
            if (!CommandSender.class.isAssignableFrom(p.getType())) {
                if (!p.isAnnotationPresent(com.axiommc.api.command.annotation.Optional.class) && !p.isAnnotationPresent(Default.class)) {
                    count++;
                }
            }
        }
        return count;
    }

    private List<String> getParamSuggestions(Method method, int argPos, String partial) {
        Parameter[] params = method.getParameters();
        int commandParamIndex = 0;
        for (Parameter param : params) {
            if (CommandSender.class.isAssignableFrom(param.getType())) continue;

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

    private List<String> filterPrefix(List<String> list, String prefix) {
        if (prefix.isEmpty()) return list;
        List<String> result = new ArrayList<>();
        for (String s : list) {
            if (s.toLowerCase().startsWith(prefix)) result.add(s);
        }
        return result;
    }
}
