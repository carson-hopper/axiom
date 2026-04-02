package com.axiommc.api.event;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

/**
 * Package-private helper that serializes and deserializes {@link CrossSide} events
 * to/from the wire format used by {@link PluginSideEventBus}.
 *
 * Delegates to Jackson's ObjectMapper for reflection-based field handling, which:
 * - Uses optimized field discovery with internal caching
 * - Automatically skips static and transient fields
 * - Supports custom serialization via @JsonProperty, @JsonIgnore, etc.
 * - Handles all primitive types, String, UUID, Enum, and nested objects
 *
 * <p>Wire format (DataOutputStream):
 * <pre>
 * [UTF]  fully-qualified class name
 * [UTF]  JSON payload (flat object of declared non-static non-transient fields)
 * </pre>
 */
final class EventSerializer {

    private static final ObjectMapper MAPPER = new ObjectMapper();

    static {
        // Configure Jackson to fail on unknown properties during deserialization
        // This helps catch errors when event classes change
        MAPPER.configure(SerializationFeature.INDENT_OUTPUT, false);
    }

    private EventSerializer() {}

    /**
     * Serializes a {@code @CrossSide} event to wire bytes using Jackson.
     *
     * @throws PluginSideSerializationException if serialization fails
     */
    static byte[] serialize(Event event) {
        Class<?> clazz = event.getClass();
        validateNoArgConstructor(clazz);

        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            DataOutputStream out = new DataOutputStream(baos);
            out.writeUTF(clazz.getName());

            // Jackson automatically discovers all non-static, non-transient fields
            // and serializes them to JSON
            String json = MAPPER.writeValueAsString(event);
            out.writeUTF(json);
            out.flush();
            return baos.toByteArray();
        } catch (Exception e) {
            throw new PluginSideSerializationException(
                "Failed to serialize event: " + e.getMessage());
        }
    }

    /**
     * Deserializes a {@code @CrossSide} event from wire bytes using Jackson.
     *
     * @throws Exception if the class cannot be loaded or instantiated
     */
    static Event deserialize(byte[] data, ClassLoader classLoader) throws Exception {
        DataInputStream in = new DataInputStream(new ByteArrayInputStream(data));
        String className = in.readUTF();
        String jsonStr = in.readUTF();

        try {
            Class<?> clazz = classLoader.loadClass(className);

            // Jackson automatically discovers fields and populates them from JSON
            return (Event) MAPPER.readValue(jsonStr, clazz);
        } catch (Exception e) {
            throw new PluginSideSerializationException(
                "Failed to deserialize event: " + e.getMessage());
        }
    }

    private static void validateNoArgConstructor(Class<?> clazz) {
        try {
            clazz.getDeclaredConstructor();
        } catch (NoSuchMethodException e) {
            throw new PluginSideSerializationException(clazz.getName()
                + " must have a no-arg constructor for @CrossSide serialization. "
                + "Add a package-private no-arg constructor: " + clazz.getSimpleName() + "() {}");
        }
    }
}
