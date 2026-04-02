package com.axiommc.fabric.chat;

import com.axiommc.api.chat.ChatComponent;

/**
 * Converts chat components with colors to console output with optional ANSI color support.
 *
 * Auto-detects terminal ANSI support and applies colors accordingly.
 * Falls back to plain text if colors are not supported.
 */
public final class ConsoleColorFormatter {

    private static final boolean ANSI_SUPPORTED = detectAnsiSupport();
    private static final String ANSI_RESET = "\033[0m";

    private ConsoleColorFormatter() {}

    /**
     * Detects whether the console supports ANSI color codes.
     */
    private static boolean detectAnsiSupport() {
        String osName = System.getProperty("os.name", "").toLowerCase();

        if (osName.contains("windows")) {
            return isWindowsTerminalOrModern();
        }

        if (System.console() == null) {
            return false;
        }

        String noColor = System.getenv("NO_COLOR");
        if (noColor != null && !noColor.isEmpty()) {
            return false;
        }

        String forceColor = System.getenv("FORCE_COLOR");
        if (forceColor != null && !forceColor.isEmpty()) {
            return true;
        }

        String term = System.getenv("TERM");
        return term != null && !term.equals("dumb");
    }

    /**
     * Checks for Windows Terminal or modern Windows 10+ ANSI support.
     */
    private static boolean isWindowsTerminalOrModern() {
        String term = System.getenv("WT_SESSION");
        if (term != null && !term.isEmpty()) {
            return true;
        }

        String winterm = System.getenv("ConEmuANSI");
        if ("ON".equals(winterm)) {
            return true;
        }

        try {
            int versionMajor = Integer.parseInt(System.getProperty("os.version", "0").split("\\.")[0]);
            return versionMajor >= 10;
        } catch (Exception e) {
            return false;
        }
    }

    /**
     * Converts a chat component to console output string.
     *
     * If ANSI is supported, returns the text with legacy color codes converted to ANSI escapes.
     * Otherwise, returns plain text without any color codes.
     *
     * @param component the chat component to format
     * @return formatted string suitable for console output
     */
    public static String format(ChatComponent component) {
        if (ANSI_SUPPORTED) {
            return formatWithAnsi(component.toLegacy());
        } else {
            return component.toPlainText();
        }
    }

    /**
     * Converts a string with legacy Minecraft color codes to ANSI-formatted string.
     *
     * @param legacyText text containing §-prefixed color and decoration codes
     * @return string with ANSI escape sequences
     */
    public static String formatWithAnsi(String legacyText) {
        if (legacyText == null) {
            return "";
        }

        StringBuilder result = new StringBuilder();
        for (int i = 0; i < legacyText.length(); i++) {
            char character = legacyText.charAt(i);

            if (character == '§' && i + 1 < legacyText.length()) {
                char code = legacyText.charAt(i + 1);
                String ansiCode = legacyCodeToAnsi(code);
                if (ansiCode != null) {
                    result.append(ansiCode);
                    i++;
                } else {
                    result.append(character);
                }
            } else {
                result.append(character);
            }
        }

        if (result.toString().contains("\033[")) {
            result.append(ANSI_RESET);
        }

        return result.toString();
    }

    /**
     * Converts a single legacy Minecraft color code to an ANSI escape sequence.
     *
     * @param code the character after § (e.g., 'c' for red)
     * @return ANSI escape sequence, or null if code is unrecognized
     */
    private static String legacyCodeToAnsi(char code) {
        return switch (code) {
            case '0' -> "\033[30m";
            case '1' -> "\033[34m";
            case '2' -> "\033[32m";
            case '3' -> "\033[36m";
            case '4' -> "\033[31m";
            case '5' -> "\033[35m";
            case '6' -> "\033[33m";
            case '7' -> "\033[37m";
            case '8' -> "\033[90m";
            case '9' -> "\033[94m";
            case 'a' -> "\033[92m";
            case 'b' -> "\033[96m";
            case 'c' -> "\033[91m";
            case 'd' -> "\033[95m";
            case 'e' -> "\033[93m";
            case 'f' -> "\033[97m";
            case 'l' -> "\033[1m";
            case 'o' -> "\033[3m";
            case 'n' -> "\033[4m";
            case 'm' -> "\033[9m";
            case 'r' -> ANSI_RESET;
            default -> null;
        };
    }

    /**
     * Returns whether ANSI colors are supported on this console.
     */
    public static boolean isAnsiSupported() {
        return ANSI_SUPPORTED;
    }
}
