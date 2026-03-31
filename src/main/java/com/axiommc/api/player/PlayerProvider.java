package com.axiommc.api.player;

import java.util.List;

public interface PlayerProvider {

    Player wrap(Object platformPlayer);

    List<Player> onlinePlayers();
}
