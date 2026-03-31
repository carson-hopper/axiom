package com.axiommc.api.command;

public interface CommandRegistry {

    void register(Command command);

    void unregister(Command command);

    void unregister(String name);

}
