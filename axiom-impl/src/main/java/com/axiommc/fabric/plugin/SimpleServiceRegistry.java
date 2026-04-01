package com.axiommc.fabric.plugin;

import com.axiommc.api.service.ServiceRegistry;
import com.axiommc.fabric.Axiom;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class SimpleServiceRegistry implements ServiceRegistry {

    private final Map<Class<?>, Object> services = new HashMap<>();

    @Override
    public <T> void register(Class<T> type, T implementation) {
        services.put(type, implementation);
        Axiom.logger().debug("Registered service: %s", type.getSimpleName());
    }

    @Override
    @SuppressWarnings("unchecked")
    public <T> Optional<T> get(Class<T> type) {
        return Optional.ofNullable((T) services.get(type));
    }

    @Override
    public <T> void unregister(Class<T> type) {
        services.remove(type);
        Axiom.logger().debug("Unregistered service: %s", type.getSimpleName());
    }

    @Override
    public boolean isRegistered(Class<?> type) {
        return services.containsKey(type);
    }
}