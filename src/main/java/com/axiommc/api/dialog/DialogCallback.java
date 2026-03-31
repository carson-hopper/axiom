package com.axiommc.api.dialog;

import com.axiommc.api.player.Player;

@FunctionalInterface
public interface DialogCallback {

    void accept(DialogResponseView view, Player player);

}
