package com.axiommc.api.player;

import java.util.Collection;
import java.util.Optional;
import java.util.UUID;

/**
 * Manager for player access and operations.
 */
public interface PlayerManager {

    Collection<? extends Player> onlinePlayers();


    Optional<? extends Player> player(UUID uuid);


    Optional<? extends Player> player(String name);
}
