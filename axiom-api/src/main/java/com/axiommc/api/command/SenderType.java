package com.axiommc.api.command;

/**
 * Specifies which type of sender can execute a command method.
 */
public enum SenderType {
    /**
     * Only players can execute this method.
     * The first parameter should be Player.
     */
    PLAYER,

    /**
     * Only the console can execute this method.
     * The first parameter should be CommandSender.
     */
    CONSOLE,

    /**
     * Both players and console can execute this method.
     */
    BOTH
}
