package com.axiommc.api.event;

import java.lang.reflect.Field;

/**
 * Base class for all events in the Axiom system.
 *
 * <p>All events must extend this class to be published and subscribed to via
 * the {@link EventBus}. Implementations should provide relevant data about
 * what occurred as public fields or getter methods.
 */
public abstract class Event {

    /**
     * Returns a string representation of this event including all declared
     * fields. Format: {@code EventName{field1=value1, field2=value2}}.
     */
    @Override
    public String toString() {
        StringBuilder result = new StringBuilder();
        result.append(getClass().getSimpleName());
        result.append('{');

        Field[] fields = getClass().getDeclaredFields();
        boolean first = true;
        for (Field field : fields) {
            if (field.isSynthetic()) {
                continue;
            }
            field.setAccessible(true);
            if (!first) {
                result.append(", ");
            }
            first = false;
            result.append(field.getName());
            result.append('=');
            try {
                Object value = field.get(this);
                result.append(value);
            } catch (IllegalAccessException _) {
                result.append("?");
            }
        }

        result.append('}');
        return result.toString();
    }
}
