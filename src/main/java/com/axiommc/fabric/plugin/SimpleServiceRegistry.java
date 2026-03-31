package com.axiommc.fabric.plugin;

import com.axiommc.api.plugin.ServiceRegistry;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;
import java.util.Map;

public class SimpleServiceRegistry implements ServiceRegistry {

    private static final Logger LOGGER = LoggerFactory.getLogger(SimpleServiceRegistry.class);

    private final Map<Class<?>, Object> services = new HashMap<>();

    @Override
    public <T> void register(Class<T> serviceClass, T implementation) {
        services.put(serviceClass, implementation);
        LOGGER.debug("Registered service: {}", serviceClass.getSimpleName());
    }

    @Override
    @SuppressWarnings("unchecked")
    public <T> T get(Class<T> serviceClass) {
        return (T) services.get(serviceClass);
    }

    @Override
    public <T> boolean has(Class<T> serviceClass) {
        return services.containsKey(serviceClass);
    }
}