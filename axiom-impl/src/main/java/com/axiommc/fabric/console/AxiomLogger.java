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
 *   <li>TRACE → Dark Gray</li>
 * </ul>
 */
public class AxiomLogger {

    private final Logger logger;

    public AxiomLogger(String name) {
        this.logger = LoggerFactory.getLogger(name);
    }

    public AxiomLogger(Class<?> clazz) {
        this.logger = LoggerFactory.getLogger(clazz);
    }

    public void info(String message, Object... args) {
        logger.info(colorize(format(message, args), ChatColor.WHITE));
    }

    public void warn(String message, Object... args) {
        logger.warn(colorize(format(message, args), ChatColor.YELLOW));
    }

    public void error(String message, Object... args) {
        logger.error(colorize(format(message, args), ChatColor.RED));
    }

    public void debug(String message, Object... args) {
        if (logger.isDebugEnabled()) {
            logger.debug(colorize(format(message, args), ChatColor.GRAY));
        }
    }

    public void trace(String message, Object... args) {
        if (logger.isTraceEnabled()) {
            logger.trace(colorize(format(message, args), ChatColor.DARK_GRAY));
        }
    }

    public void info(ChatComponent component) {
        logger.info(ConsoleColorFormatter.format(component));
    }

    public void warn(ChatComponent component) {
        logger.warn(ConsoleColorFormatter.format(component));
    }

    public void error(ChatComponent component) {
        logger.error(ConsoleColorFormatter.format(component));
    }

    /**
     * Returns the underlying SLF4J logger.
     */
    public Logger logger() {
        return logger;
    }

    private String colorize(String message, ChatColor color) {
        ChatComponent component = ChatComponent.text(message).color(color);
        return ConsoleColorFormatter.format(component);
    }

    /**
     * Replaces SLF4J-style {} placeholders with arguments.
     */
    private String format(String message, Object... args) {
        if (args == null || args.length == 0) {
            return message;
        }
        StringBuilder result = new StringBuilder();
        int argIndex = 0;
        int i = 0;
        while (i < message.length()) {
            if (i + 1 < message.length()
                && message.charAt(i) == '{'
                && message.charAt(i + 1) == '}') {
                if (argIndex < args.length) {
                    result.append(args[argIndex++]);
                } else {
                    result.append("{}");
                }
                i += 2;
            } else {
                result.append(message.charAt(i));
                i++;
            }
        }
        return result.toString();
    }
}
