package com.axiommc.fabric.bossbar;

import com.axiommc.api.bossbar.BossBar;
import com.axiommc.api.bossbar.BossBarColor;
import com.axiommc.api.bossbar.BossBarStyle;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerBossEvent;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.BossEvent;

public record FabricBossBar(ServerBossEvent bossEvent) implements BossBar {

    @Override
    public void title(ChatComponent title) {
        bossEvent.setName(Component.literal(title.toString()));
    }

    @Override
    public void process(float progress) {
        bossEvent.setProgress(Math.max(0f, Math.min(1f, progress)));
    }

    @Override
    public void color(BossBarColor color) {
        bossEvent.setColor(mapColor(color));
    }

    @Override
    public void style(BossBarStyle style) {
        bossEvent.setOverlay(mapStyle(style));
    }

    @Override
    public void addPlayer(Player player) {
        if (player instanceof FabricPlayer fabricPlayer) {
            bossEvent.addPlayer(fabricPlayer.player());
        }
    }

    @Override
    public void removePlayer(Player player) {
        if (player instanceof FabricPlayer fabricPlayer) {
            bossEvent.removePlayer(fabricPlayer.player());
        }
    }

    @Override
    public void destroy() {
        bossEvent.removeAllPlayers();
        bossEvent.setVisible(false);
    }

    @Override
    public boolean isActive() {
        return bossEvent.isVisible();
    }

    private BossEvent.BossBarColor mapColor(BossBarColor color) {
        return switch (color) {
            case PINK -> BossEvent.BossBarColor.PINK;
            case BLUE -> BossEvent.BossBarColor.BLUE;
            case RED -> BossEvent.BossBarColor.RED;
            case GREEN -> BossEvent.BossBarColor.GREEN;
            case YELLOW -> BossEvent.BossBarColor.YELLOW;
            case PURPLE -> BossEvent.BossBarColor.PURPLE;
            case WHITE -> BossEvent.BossBarColor.WHITE;
        };
    }

    private BossEvent.BossBarOverlay mapStyle(BossBarStyle style) {
        return switch (style) {
            case PROGRESS -> BossEvent.BossBarOverlay.PROGRESS;
            case NOTCHED_6 -> BossEvent.BossBarOverlay.NOTCHED_6;
            case NOTCHED_10 -> BossEvent.BossBarOverlay.NOTCHED_10;
            case NOTCHED_12 -> BossEvent.BossBarOverlay.NOTCHED_12;
            case NOTCHED_20 -> BossEvent.BossBarOverlay.NOTCHED_20;
        };
    }
}
