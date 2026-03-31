package com.axiommc.api.service;

import java.util.Optional;

public interface ServiceRegistry {

    <T> void register(Class<T> type, T implementation);

    <T> Optional<T> get(Class<T> type);

    <T> void unregister(Class<T> type);

    boolean isRegistered(Class<?> type);
}
