package com.axiommc.api.sound;

public interface SoundKey {

    String key();

    static SoundKey of(String key) {
        return () -> key;
    }

}
