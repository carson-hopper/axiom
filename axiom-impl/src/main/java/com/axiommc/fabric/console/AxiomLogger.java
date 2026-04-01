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
 *   <li>INFO  → Blue</li>
 *   <li>WARN  → Yellow</li>
 *   <li>ERROR → Red</li>
 *   <li>DEBUG → Gray</li>
 *   <li>TRACE → Dark Gray</li>
 * </ul>
 */
public class AxiomLogger {

    private static final ChatColor INFO_COLOR = ChatColor.BLUE;
    private static final ChatColor WARN_COLOR = ChatColor.YELLOW;
    private static final ChatColor ERROR_COLOR = ChatColor.RED;
    private static final ChatColor DEBUG_COLOR = ChatColor.GRAY;
    private static final ChatColor TRACE_COLOR = ChatColor.DARK_GRAY;

    private final Logger delegate;

    public AxiomLogger(String name) {
        this.delegate = LoggerFactory.getLogger(name);
    }

    public AxiomLogger(Class<?> clazz) {
        this.delegate = LoggerFactory.getLogger(clazz);
    }

    public void info(String message, Object... args) {
        String formatted = format(message, args);
        delegate.info(colorize(formatted, INFO_COLOR));
    }

    public void warn(String message, Object... args) {
        String formatted = format(message, args);
        delegate.warn(colorize(formatted, WARN_COLOR));
    }

    public void error(String message, Object... args) {
        String formatted = format(message, args);
        delegate.error(colorize(formatted, ERROR_COLOR));
    }

    public void debug(String message, Object... args) {
        if (delegate.isDebugEnabled()) {
            String formatted = format(message, args);
            delegate.debug(colorize(formatted, DEBUG_COLOR));
        }
    }

    public void trace(String message, Object... args) {
        if (delegate.isTraceEnabled()) {
            String formatted = format(message, args);
            delegate.trace(colorize(formatted, TRACE_COLOR));
        }
    }

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
        StringBuilder sb = new StringBuilder();
        int argIndex = 0;
        int i = 0;
        while (i < message.length()) {
            if (i + 1 < message.length()
                    && message.charAt(i) == '{'
                    && message.charAt(i + 1) == '}') {
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
