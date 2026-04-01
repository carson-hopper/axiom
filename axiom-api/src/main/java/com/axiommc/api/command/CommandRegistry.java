package com.axiommc.api.command;

/**
 * Registry for managing command registration and unregistration.
 *
 * <p>Commands must be registered with this registry to become available
 * to command senders. This interface provides methods to register and
 * unregister commands by instance or name.
 */
public interface CommandRegistry {

    /**
     * Registers a command.
     *
     * @param command the command to register (must not be null)
     */
    void register(Object command);

    /**
     * Unregisters a command.
     *
     * @param command the command to unregister (must not be null)
     */
    void unregister(Object command);

    /**
     * Unregisters a command by name.
     *
     * @param name the command name (must not be null)
     */
    void unregister(String name);

}
