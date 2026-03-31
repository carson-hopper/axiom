package com.axiommc.fabric.player;

import com.axiommc.api.player.Player;
import com.axiommc.api.player.PlayerManager;
import net.minecraft.server.MinecraftServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collection;
import java.util.Collections;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;

public class FabricPlayerProvider implements PlayerManager {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricPlayerProvider.class);
    private MinecraftServer server;

    public void setServer(MinecraftServer server) {
        this.server = server;
        LOGGER.debug("FabricPlayerProvider initialized with MinecraftServer");
    }

    @Override
    public Optional<FabricPlayer> player(String name) {
        if (server == null) {
            return Optional.empty();
        }
        return server.getPlayerList().getPlayers().stream()
                .filter(p -> p.getName().getString().equalsIgnoreCase(name))
                .findFirst()
                .map(FabricPlayer::new);
    }

    @Override
    public Optional<FabricPlayer> player(UUID uuid) {
        if (server == null) {
            return Optional.empty();
        }
        return Optional.ofNullable(server.getPlayerList().getPlayer(uuid))
                .map(FabricPlayer::new);
    }

    @Override
    public Collection<FabricPlayer> onlinePlayers() {
        if (server == null) {
            return Collections.emptyList();
        }
        return server.getPlayerList().getPlayers().stream()
                .map(FabricPlayer::new)
                .collect(Collectors.toList());
    }

    public int count() {
        if (server == null) {
            return 0;
        }
        return server.getPlayerList().getPlayers().size();
    }
}