package com.axiommc.fabric.command.parser;

import com.axiommc.api.command.parser.ArgParser;
import com.axiommc.api.command.parser.ArgParseException;
import com.axiommc.api.math.Vector3;
import java.util.Arrays;
import java.util.List;

public class Vector3ArgParser implements ArgParser<Vector3> {

    @Override
    public Vector3 parse(String input) throws ArgParseException {
        String[] parts = input.split("[,\\s]+");
        
        if (parts.length == 1) {
            try {
                double value = Double.parseDouble(parts[0]);
                return new Vector3(value, 0, 0);
            } catch (NumberFormatException e) {
                throw new ArgParseException("Invalid number: " + parts[0]);
            }
        }
        
        if (parts.length == 3) {
            try {
                double x = Double.parseDouble(parts[0]);
                double y = Double.parseDouble(parts[1]);
                double z = Double.parseDouble(parts[2]);
                return new Vector3(x, y, z);
            } catch (NumberFormatException e) {
                throw new ArgParseException("Invalid coordinates: " + input);
            }
        }
        
        throw new ArgParseException("Vector3 requires 1 or 3 values (got " + parts.length + ")");
    }

    @Override
    public List<String> suggest(String partial) {
        return Arrays.asList("<x,y,z>", "<x>");
    }
}
