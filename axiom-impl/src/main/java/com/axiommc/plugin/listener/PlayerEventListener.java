package com.axiommc.plugin.listener;

import com.axiommc.api.event.EventListener;

/**
 * Example class-based event listener demonstrating the {@code @EventHandler}
 * annotation pattern. Register with {@code eventBus.registerListener(new PlayerEventListener())}.
 */
public class PlayerEventListener implements EventListener {

    /*
     @EventHandler
    public void onJoin(PlayerJoinEvent.Post event) {
        Axiom.logger().info("{} joined the server", event.player().name());
        event.player()
            .sendMessage(
                ChatComponent.textf("Welcome, {}!", event.player().name()).color(ChatColor.GREEN));
    }

    @EventHandler
    public void onLeave(PlayerLeaveEvent event) {
        Axiom.logger().info("{} left the server", event.player().name());
    }

    @EventHandler(priority = EventPriority.HIGH)
    public void onChat(PlayerChatEvent.Pre event) {
        String message = event.message();
        event.player()
            .sendMessage(ChatComponent.text("[Chat] " + event.player().name() + ": " + message)
                .color(ChatColor.AQUA));
    }

    @EventHandler
    public void onDeath(PlayerDeathEvent.Death event) {
        event.player()
            .sendMessage(
                ChatComponent.text("You died: " + event.deathMessage()).color(ChatColor.RED));
    }

    @EventHandler
    public void onDamage(PlayerDamageEvent event) {
        if (event.damage() > 10.0) {
            event.player()
                .sendMessage(ChatComponent.text(
                        "Heavy hit! " + String.format("%.1f", event.damage()) + " damage")
                    .color(ChatColor.RED));
        }
    }
     */
}
