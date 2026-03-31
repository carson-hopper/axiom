package com.axiommc.api.plugin;

/**
 * Registry for plugin services.
 */
public interface ServiceRegistry {

    /**
     * Register a service implementation.
     */
    <T> void register(Class<T> serviceClass, T implementation);

    /**
     * Get a registered service.
     */
    <T> T get(Class<T> serviceClass);

    /**
     * Check if a service is registered.
     */
    <T> boolean has(Class<T> serviceClass);

}
