package com.axiommc.api.bossbar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;

public interface BossBar {

    void title(ChatComponent title);

    void process(float progress);

    void color(BossBarColor color);

    void style(BossBarStyle style);

    void addPlayer(Player player);

    void removePlayer(Player player);

    void destroy();

    boolean isActive();

    record Spec(ChatComponent title, float progress, BossBarColor color, BossBarStyle style) {

        public static Spec of(ChatComponent title) {
            return new Spec(title, 1.0f, BossBarColor.PURPLE, BossBarStyle.PROGRESS);
        }

        public Spec title(ChatComponent title) {
            return new Spec(title, progress, color, style);
        }

        public Spec progress(float progress) {
            return new Spec(title, progress, color, style);
        }

        public Spec color(BossBarColor color) {
            return new Spec(title, progress, color, style);
        }

        public Spec style(BossBarStyle style) {
            return new Spec(title, progress, color, style);
        }
    }
}
