package com.axiommc.api.command.parser;

public class ArgParseException extends Exception {

    public ArgParseException(String message) {
        super(message);
    }

    public ArgParseException(String message, Throwable cause) {
        super(message, cause);
    }
}
