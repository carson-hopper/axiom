package com.axiommc.api.dialog;

import com.axiommc.api.player.Player;

public interface DialogManager {

    void show(Player player, Dialog dialog);

    void close(Player player);
}
