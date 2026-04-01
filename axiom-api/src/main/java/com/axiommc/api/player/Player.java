package com.axiommc.api.player;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.entity.LivingEntity;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.Server;

/**
 * Represents a player connected to the server.
 *
 * <p>Players are living entities that can send commands, receive messages,
 * and interact with GUIs. This interface extends both {@link LivingEntity}
 * and {@link CommandSender}.
 */
public interface Player extends LivingEntity, CommandSender {

    /**
     * Teleports this player to a location on another server.
     *
     * @param server the destination server
     * @param location the destination location
     */
    void teleport(Server server, Location location);

    /**
     * Transfers this player to another server.
     *
     * @param server the destination server
     */
    void transfer(Server server);

    /**
     * Checks whether this player is currently online.
     *
     * @return true if the player is online, false otherwise
     */
    boolean isOnline();

    /**
     * Kicks this player from the server with a reason message.
     *
     * @param reason the kick reason to display to the player
     */
    void kick(String reason);

    /**
     * Shows a title on the player's screen.
     *
     * <p>The title consists of a main title and optional subtitle. The
     * fade-in, stay, and fade-out durations control the display timing
     * in ticks.
     *
     * @param title the main title (must not be null)
     * @param subtitle the subtitle or null for none
     * @param fadeIn the fade-in duration in ticks
     * @param stay the duration the title stays visible in ticks
     * @param fadeOut the fade-out duration in ticks
     * @param ttl the total lifetime of the title in ticks
     */
    void showTitle(ChatComponent title, ChatComponent subtitle, int fadeIn, int stay,
                   int fadeOut, int ttl);

    /**
     * Clears any title currently shown to this player.
     */
    void clearTitle();

    /**
     * Sends an action bar message to this player.
     *
     * <p>The action bar is displayed above the hotbar.
     *
     * @param component the message component
     */
    void sendActionBar(ChatComponent component);

    /**
     * Plays a sound for this player.
     *
     * @param sound the sound to play
     * @param volume the volume (0.0 to 1.0+)
     * @param pitch the pitch (0.5 to 2.0 typical)
     */
    void playSound(SoundKey sound, float volume, float pitch);

    /**
     * Opens a GUI for this player.
     *
     * @param gui the GUI to open
     */
    void openGui(Gui gui);

}
