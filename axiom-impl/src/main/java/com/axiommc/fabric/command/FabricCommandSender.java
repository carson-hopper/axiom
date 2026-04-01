package com.axiommc.fabric.command;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.commands.CommandSourceStack;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerPlayer;

import java.util.Optional;

public record FabricCommandSender(CommandSourceStack source) implements CommandSender {

    @Override
    public void sendMessage(String message) {
        source.sendSuccess(() -> Component.literal(message), false);
    }

    @Override
    public void sendMessage(ChatComponent component) {
        Component minecraftComponent = new FabricComponentSerializer().serialize(component);
        source.sendSuccess(() -> minecraftComponent, false);
    }

    @Override
    public boolean hasPermission(String permission) {
        // For now, just check if sender is a player or console
        // Console always has permissions, players always allowed (permission check done in command execution)
        return true;
    }

    @Override
    public boolean isPlayer() {
        return source.getEntity() instanceof ServerPlayer;
    }

    @Override
    public Optional<Player> asPlayer() {
        if (source.getEntity() instanceof ServerPlayer serverPlayer) {
            return Optional.of(new FabricPlayer(serverPlayer));
        }
        return Optional.empty();
    }
}