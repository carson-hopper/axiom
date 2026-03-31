package com.axiommc.api.player;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.Server;

import java.util.UUID;

public interface Player extends LivingEntity, CommandSender {

    void teleport(Server server, Location location);
    void transfer(Server server);

    boolean isOnline();

    void kick(String reason);

    void showTitle(ChatComponent title, ChatComponent subtitle, int fadeIn, int stay, int fadeOut, int ttl);
    void clearTitle();
    void sendActionBar(ChatComponent component);

    void playSound(SoundKey sound, float volume, float pitch);

    void openGui(Gui gui);

}
