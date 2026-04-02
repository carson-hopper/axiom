package com.axiommc.fabric.bossbar;

import com.axiommc.api.bossbar.BossBar;
import com.axiommc.api.bossbar.BossBarManager;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.ServerBossEvent;
import net.minecraft.world.BossEvent;

public class FabricBossBarManager implements BossBarManager {

    private final Map<UUID, FabricBossBar> bossBars = new HashMap<>();

    @Override
    public BossBar create(BossBar.Spec spec) {
        UUID id = UUID.randomUUID();

        Component title = Component.literal(spec.title().toString());
        BossEvent.BossBarColor mcColor = mapColor(spec.color());
        BossEvent.BossBarOverlay mcStyle = mapStyle(spec.style());

        ServerBossEvent serverBossEvent = new ServerBossEvent(id, title, mcColor, mcStyle);
        serverBossEvent.setProgress(Math.max(0f, Math.min(1f, spec.progress())));
        serverBossEvent.setVisible(true);

        FabricBossBar fabricBossBar = new FabricBossBar(serverBossEvent);
        bossBars.put(id, fabricBossBar);

        return fabricBossBar;
    }

    public void unregister(UUID id) {
        bossBars.remove(id);
    }

    private BossEvent.BossBarColor mapColor(com.axiommc.api.bossbar.BossBarColor color) {
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

    private BossEvent.BossBarOverlay mapStyle(com.axiommc.api.bossbar.BossBarStyle style) {
        return switch (style) {
            case PROGRESS -> BossEvent.BossBarOverlay.PROGRESS;
            case NOTCHED_6 -> BossEvent.BossBarOverlay.NOTCHED_6;
            case NOTCHED_10 -> BossEvent.BossBarOverlay.NOTCHED_10;
            case NOTCHED_12 -> BossEvent.BossBarOverlay.NOTCHED_12;
            case NOTCHED_20 -> BossEvent.BossBarOverlay.NOTCHED_20;
        };
    }
}
