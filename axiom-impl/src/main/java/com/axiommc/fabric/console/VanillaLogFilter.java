package com.axiommc.fabric.console;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.core.Appender;
import org.apache.logging.log4j.core.LogEvent;
import org.apache.logging.log4j.core.Logger;
import org.apache.logging.log4j.core.filter.AbstractFilter;
import org.apache.logging.log4j.core.impl.Log4jLogEvent;

/**
 * Log4j filter installed on the root logger that:
 * <ul>
 *   <li>Suppresses noisy vanilla join/UUID messages</li>
 *   <li>Rewrites the thread name to "Axiom" for Axiom logger events</li>
 * </ul>
 *
 * <p>Thread rewriting is done by returning {@link Result#DENY} on the
 * original event and directly logging a rewritten copy. This avoids
 * mutating the actual thread name and is fully thread-safe.
 */
public final class VanillaLogFilter extends AbstractFilter {

    private static final String AXIOM_LOGGER_PREFIX = "axiom";
    private static final String AXIOM_THREAD_NAME = "Axiom";
    private volatile boolean rewriting = false;

    private VanillaLogFilter() {
        super(Result.DENY, Result.NEUTRAL);
    }

    @Override
    public Result filter(LogEvent event) {
        // Prevent recursion from our own rewritten events
        if (rewriting) {
            return Result.NEUTRAL;
        }

        String message = event.getMessage().getFormattedMessage();

        // Suppress vanilla join messages
        if (message.contains("UUID of player")
                || message.contains("logged in with entity id")) {
            return Result.DENY;
        }

        // Rewrite thread name for Axiom loggers
        String loggerName = event.getLoggerName();
        if (loggerName != null
                && loggerName.startsWith(AXIOM_LOGGER_PREFIX)
                && !AXIOM_THREAD_NAME.equals(event.getThreadName())) {
            LogEvent rewritten = new Log4jLogEvent.Builder(event)
                    .setThreadName(AXIOM_THREAD_NAME)
                    .build();
            rewriting = true;
            try {
                Logger rootLogger = (Logger) LogManager.getRootLogger();
                for (Appender appender : rootLogger.getAppenders().values()) {
                    appender.append(rewritten);
                }
            } finally {
                rewriting = false;
            }
            return Result.DENY;
        }

        return Result.NEUTRAL;
    }

    /**
     * Installs this filter on the root Log4j logger.
     * Call once during mod initialization.
     */
    public static void install() {
        Logger rootLogger = (Logger) LogManager.getRootLogger();
        rootLogger.addFilter(new VanillaLogFilter());
    }
}
