package com.axiommc.api.chat;

/**
 * Represents a color for chat text, supporting both RGB values and legacy Minecraft color codes.
 *
 * @param rgb the RGB color value (0x000000 to 0xFFFFFF)
 * @param legacyCode '\0' if not a named color; otherwise a legacy Minecraft color code ('0'-'f')
 * @param namedColor true if this color has a corresponding legacy code
 */
public record ChatColor(int rgb, char legacyCode, boolean namedColor) {

    // Named colors with packed-RGB values and legacy § codes
    public static final ChatColor BLACK = named(0x000000, '0');
    public static final ChatColor DARK_BLUE = named(0x0000AA, '1');
    public static final ChatColor DARK_GREEN = named(0x00AA00, '2');
    public static final ChatColor DARK_AQUA = named(0x00AAAA, '3');
    public static final ChatColor DARK_RED = named(0xAA0000, '4');
    public static final ChatColor DARK_PURPLE = named(0xAA00AA, '5');
    public static final ChatColor GOLD = named(0xFFAA00, '6');
    public static final ChatColor GRAY = named(0xAAAAAA, '7');
    public static final ChatColor DARK_GRAY = named(0x555555, '8');
    public static final ChatColor BLUE = named(0x5555FF, '9');
    public static final ChatColor GREEN = named(0x55FF55, 'a');
    public static final ChatColor AQUA = named(0x55FFFF, 'b');
    public static final ChatColor RED = named(0xFF5555, 'c');
    public static final ChatColor LIGHT_PURPLE = named(0xFF55FF, 'd');
    public static final ChatColor YELLOW = named(0xFFFF55, 'e');
    public static final ChatColor WHITE = named(0xFFFFFF, 'f');

    private static ChatColor named(int rgb, char code) {
        return new ChatColor(rgb, code, true);
    }

    // ── Factories ──────────────────────────────────────────────────────────

    /**
     * Creates a color from an RGB value.
     *
     * @param rgb the RGB value (0x000000 to 0xFFFFFF)
     * @return a new ChatColor with the given RGB value
     */
    public static ChatColor color(int rgb) {
        return new ChatColor(rgb & 0xFFFFFF, '\0', false);
    }

    /**
     * Creates a color from separate red, green, and blue components.
     *
     * @param r the red component (0-255)
     * @param g the green component (0-255)
     * @param b the blue component (0-255)
     * @return a new ChatColor with the specified RGB values
     */
    public static ChatColor color(int r, int g, int b) {
        return color(((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF));
    }

    /**
     * Parses a color from a hexadecimal string.
     *
     * @param hex the hex color string (with or without "#" prefix, e.g., "FF0000" or "#FF0000")
     * @return a new ChatColor parsed from the hex string
     * @throws IllegalArgumentException if the hex string is invalid
     */
    public static ChatColor color(String hex) {
        String s = hex.startsWith("#") ? hex.substring(1) : hex;
        try {
            return color(Integer.parseInt(s, 16));
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Invalid hex color: " + hex, e);
        }
    }

    // ── Accessors ──────────────────────────────────────────────────────────

    /** Returns the RGB color value. */
    public int value() {
        return rgb;
    }

    /** Returns this color as a hexadecimal string prefixed with "#". */
    public String asHexString() {
        return String.format("#%06X", rgb);
    }

    /** Returns true if this color has a corresponding legacy Minecraft color code. */
    public boolean hasLegacyCode() {
        return namedColor;
    }

    /**
     * Returns the legacy § code character. Only valid when hasLegacyCode() is true.
     */
    @Override
    public char legacyCode() {
        if (!namedColor) {
            throw new IllegalStateException("RGB-only ChatColor has no legacy code");
        }
        return legacyCode;
    }
}
