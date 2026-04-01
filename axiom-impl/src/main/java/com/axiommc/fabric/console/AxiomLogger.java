package com.axiommc.fabric.console;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.fabric.chat.ConsoleColorFormatter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Custom logger that automatically colorizes output based on log level.
 *
 * <ul>
 *   <li>INFO  → White</li>
 *   <li>WARN  → Yellow</li>
 *   <li>ERROR → Red</li>
 *   <li>DEBUG → Gray</li>
 * </ul>
 */
public class AxiomLogger {

    private final Logger delegate;

    public AxiomLogger(String name) {
        this.delegate = LoggerFactory.getLogger(name);
    }

    public AxiomLogger(Class<?> clazz) {
        this.delegate = LoggerFactory.getLogger(clazz);
    }

    public void info(String message, Object... args) {
        String formatted = format(message, args);
        delegate.info(ConsoleColorFormatter.formatWithAnsi("§f" + formatted));
    }

    public void warn(String message, Object... args) {
        String formatted = format(message, args);
        delegate.warn(ConsoleColorFormatter.formatWithAnsi("§e" + formatted));
    }

    public void error(String message, Object... args) {
        String formatted = format(message, args);
        delegate.error(ConsoleColorFormatter.formatWithAnsi("§c" + formatted));
    }

    public void debug(String message, Object... args) {
        if (delegate.isDebugEnabled()) {
            String formatted = format(message, args);
            delegate.debug(ConsoleColorFormatter.formatWithAnsi("§7" + formatted));
        }
    }

    /**
     * Sends a ChatComponent message to the console with its own colors.
     */
    public void info(ChatComponent component) {
        delegate.info(ConsoleColorFormatter.format(component));
    }

    public void warn(ChatComponent component) {
        delegate.warn(ConsoleColorFormatter.format(component));
    }

    public void error(ChatComponent component) {
        delegate.error(ConsoleColorFormatter.format(component));
    }

    /**
     * Returns the underlying SLF4J logger.
     */
    public Logger delegate() {
        return delegate;
    }

    /**
     * Replaces SLF4J-style {} placeholders with arguments.
     */
    private String format(String message, Object... args) {
        if (args == null || args.length == 0) {
            return message;
        }
        StringBuilder sb = new StringBuilder();
        int argIndex = 0;
        int i = 0;
        while (i < message.length()) {
            if (i + 1 < message.length() && message.charAt(i) == '{' && message.charAt(i + 1) == '}') {
                if (argIndex < args.length) {
                    sb.append(args[argIndex++]);
                } else {
                    sb.append("{}");
                }
                i += 2;
            } else {
                sb.append(message.charAt(i));
                i++;
            }
        }
        return sb.toString();
    }
}
