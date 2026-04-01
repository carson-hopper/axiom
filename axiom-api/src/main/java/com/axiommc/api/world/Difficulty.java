package com.axiommc.api.world;

import java.util.HashMap;
import java.util.Map;

public enum Difficulty {
    PEACEFUL(0, "peaceful"),
    EASY(1, "easy"),
    NORMAL(2, "normal"),
    HARD(3, "hard");

    private final int id;
    private final String key;

    private static final Map<String, Difficulty> BY_NAME;
    private static final Map<Integer, Difficulty> BY_ID;

    static {
        Map<String, Difficulty> nameMap = new HashMap<>();
        Map<Integer, Difficulty> idMap = new HashMap<>();
        for (Difficulty difficulty : values()) {
            nameMap.put(difficulty.key, difficulty);
            idMap.put(difficulty.id, difficulty);
        }
        BY_NAME = Map.copyOf(nameMap);
        BY_ID = Map.copyOf(idMap);
    }

    Difficulty(final int id, final String key) {
        this.id = id;
        this.key = key;
    }

    public int id() {
        return this.id;
    }

    public String key() {
        return this.key;
    }

    public static Difficulty of(final String name) {
        return BY_NAME.get(name);
    }

    public static Difficulty of(final int id) {
        return BY_ID.get(id);
    }
}