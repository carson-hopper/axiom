package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.event.EventBus;
import com.axiommc.api.event.EventPriority;
import com.axiommc.api.event.block.BlockEvent;
import com.axiommc.api.event.block.RedstoneEvent;
import com.axiommc.api.event.command.CommandExecuteEvent;
import com.axiommc.api.event.entity.EntityEvent;
import com.axiommc.api.event.inventory.InventoryEvent;
import com.axiommc.api.event.player.PlayerAdvancementEvent;
import com.axiommc.api.event.player.PlayerAnimationEvent;
import com.axiommc.api.event.player.PlayerBedEvent;
import com.axiommc.api.event.player.PlayerChatEvent;
import com.axiommc.api.event.player.PlayerClientBrandEvent;
import com.axiommc.api.event.player.PlayerDamageEvent;
import com.axiommc.api.event.player.PlayerDeathEvent;
import com.axiommc.api.event.player.PlayerExperienceEvent;
import com.axiommc.api.event.player.PlayerGameModeChangeEvent;
import com.axiommc.api.event.player.PlayerInputEvent;
import com.axiommc.api.event.player.PlayerInventoryEvent;
import com.axiommc.api.event.player.PlayerItemEvent;
import com.axiommc.api.event.player.PlayerJoinEvent;
import com.axiommc.api.event.player.PlayerLeaveEvent;
import com.axiommc.api.event.player.PlayerPositionEvent;
import com.axiommc.api.event.player.PlayerSettingsChangedEvent;
import com.axiommc.api.event.player.PlayerSwapHandItemsEvent;
import com.axiommc.api.event.player.PlayerToggleEvent;
import com.axiommc.api.event.vehicle.VehicleEvent;
import com.axiommc.api.event.world.WorldChunkEvent;
import com.axiommc.api.event.world.WorldLifecycleEvent;
import com.axiommc.api.event.world.WorldTimeSkipEvent;
import com.axiommc.api.event.world.WorldWeatherEvent;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Player;
import com.axiommc.fabric.Axiom;
import java.util.Collections;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manages event debug subscriptions and broadcasts debug messages to
 * players who have opted in.
 */
public final class EventDebugListener {

    /** Maps player UUID to their set of enabled event names (or "*" for all). */
    private static final ConcurrentHashMap<UUID, Set<String>> PLAYER_FILTERS =
        new ConcurrentHashMap<>();

    /** All known event names for tab completion. */
    private static final Set<String> ALL_EVENT_NAMES = new java.util.TreeSet<>();

    private EventDebugListener() {}

    /** Enable all events for a player. */
    public static void enableAll(UUID uuid) {
        PLAYER_FILTERS
            .computeIfAbsent(uuid, k -> Collections.newSetFromMap(new ConcurrentHashMap<>()))
            .add("*");
    }

    /** Disable all events for a player. */
    public static void disableAll(UUID uuid) {
        PLAYER_FILTERS.remove(uuid);
    }

    /** Toggle a specific event for a player. Returns true if now enabled. */
    public static boolean toggle(UUID uuid, String eventName) {
        Set<String> filters = PLAYER_FILTERS.computeIfAbsent(
            uuid, k -> Collections.newSetFromMap(new ConcurrentHashMap<>()));
        filters.remove("*");
        if (filters.contains(eventName)) {
            filters.remove(eventName);
            if (filters.isEmpty()) {
                PLAYER_FILTERS.remove(uuid);
            }
            return false;
        } else {
            filters.add(eventName);
            return true;
        }
    }

    /** Check if a player is listening for a specific event name. */
    public static boolean isEnabled(UUID uuid, String eventName) {
        Set<String> filters = PLAYER_FILTERS.get(uuid);
        if (filters == null) {
            return false;
        }
        return filters.contains("*") || filters.contains(eventName);
    }

    /** Check if a player has any debug enabled. */
    public static boolean isEnabled(UUID uuid) {
        return PLAYER_FILTERS.containsKey(uuid);
    }

    /** Returns all known event names for tab completion. */
    public static java.util.List<String> eventNames() {
        return new java.util.ArrayList<>(ALL_EVENT_NAMES);
    }

    /**
     * Registers debug listeners for all key events on the given bus.
     *
     * @param eventBus the event bus to subscribe to
     */
    public static void registerAll(EventBus eventBus) {
        registerPlayerJoinEvents(eventBus);
        registerPlayerLeaveEvent(eventBus);
        registerPlayerChatEvents(eventBus);
        registerPlayerPositionEvents(eventBus);
        registerPlayerDamageEvent(eventBus);
        registerPlayerDeathEvents(eventBus);
        registerPlayerItemEvents(eventBus);
        registerPlayerToggleEvents(eventBus);
        registerPlayerInventoryEvents(eventBus);
        registerPlayerGameModeEvent(eventBus);
        registerPlayerSwapHandItemsEvent(eventBus);
        registerPlayerAdvancementEvent(eventBus);
        registerPlayerAnimationEvent(eventBus);
        registerPlayerBedEvents(eventBus);
        registerPlayerExperienceEvents(eventBus);
        registerPlayerInputEvent(eventBus);
        registerVehicleEvents(eventBus);
        registerBlockEvents(eventBus);
        // registerEntityEvents(eventBus);
        registerInventoryEvents(eventBus);
        registerCommandExecuteEvent(eventBus);
        registerPlayerClientBrandEvent(eventBus);
        registerPlayerSettingsChangedEvent(eventBus);
        registerWorldEvents(eventBus);
        registerRedstoneEvents(eventBus);
    }

    // ── Player join ────────────────────────────────────────────────

    private static void registerPlayerJoinEvents(EventBus eventBus) {
        eventBus.listen(PlayerJoinEvent.Connecting.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerJoinEvent.Connecting",
                    "player: " + event.player().name());
            });

        eventBus.listen(PlayerJoinEvent.Post.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerJoinEvent.Post",
                    "player: " + event.player().name() + " | server: "
                        + event.server().id());
            });
    }

    // ── Player leave ───────────────────────────────────────────────

    private static void registerPlayerLeaveEvent(EventBus eventBus) {
        eventBus.listen(PlayerLeaveEvent.class).priority(EventPriority.MONITOR).handler(event -> {
            sendDebug(
                event.player(), "PlayerLeaveEvent", "player: " + event.player().name());
        });
    }

    // ── Player chat ────────────────────────────────────────────────

    private static void registerPlayerChatEvents(EventBus eventBus) {
        eventBus.listen(PlayerChatEvent.Pre.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerChatEvent.Pre", "msg: " + event.message());
            });
    }

    // ── Player position ────────────────────────────────────────────

    private static void registerPlayerPositionEvents(EventBus eventBus) {
        eventBus.listen(PlayerPositionEvent.Move.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                Vector3 from = event.from();
                Vector3 to = event.to();
                double dx = to.x() - from.x();
                double dy = to.y() - from.y();
                double dz = to.z() - from.z();
                double distSq = dx * dx + dy * dy + dz * dz;
                if (distSq >= 1.0) {
                    sendDebug(
                        event.player(),
                        "PlayerPositionEvent.Move",
                        "from: " + formatVec(from) + " | to: " + formatVec(to));
                }
            });

        eventBus.listen(PlayerPositionEvent.Teleport.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerPositionEvent.Teleport",
                    "from: " + event.from() + " | to: " + event.to());
            });
    }

    // ── Player damage ──────────────────────────────────────────────

    private static void registerPlayerDamageEvent(EventBus eventBus) {
        eventBus.listen(PlayerDamageEvent.class).priority(EventPriority.MONITOR).handler(event -> {
            sendDebug(
                event.player(),
                "PlayerDamageEvent",
                "damage: " + String.format("%.1f", event.damage()) + " | cause: " + event.cause());
        });
    }

    // ── Player death ───────────────────────────────────────────────

    private static void registerPlayerDeathEvents(EventBus eventBus) {
        eventBus.listen(PlayerDeathEvent.Death.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerDeathEvent.Death", "msg: " + event.deathMessage());
            });

        eventBus.listen(PlayerDeathEvent.Respawn.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerDeathEvent.Respawn",
                    "player: " + event.player().name());
            });
    }

    // ── Player item ────────────────────────────────────────────────

    private static void registerPlayerItemEvents(EventBus eventBus) {
        eventBus.listen(PlayerItemEvent.Drop.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerItemEvent.Drop",
                    "item: " + event.itemStack().item().key() + " x"
                        + event.itemStack().count());
            });

        eventBus.listen(PlayerItemEvent.Pickup.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerItemEvent.Pickup",
                    "item: " + event.itemStack().item().key() + " x"
                        + event.itemStack().count());
            });
    }

    // ── Player toggle ──────────────────────────────────────────────

    private static void registerPlayerToggleEvents(EventBus eventBus) {
        eventBus.listen(PlayerToggleEvent.Sneak.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(), "PlayerToggleEvent.Sneak", "sneaking: " + event.isSneaking());
            });

        eventBus.listen(PlayerToggleEvent.Sprint.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerToggleEvent.Sprint",
                    "sprinting: " + event.isSprinting());
            });

        eventBus.listen(PlayerToggleEvent.Flight.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(), "PlayerToggleEvent.Flight", "flying: " + event.isFlying());
            });
    }

    // ── Player inventory ───────────────────────────────────────────

    private static void registerPlayerInventoryEvents(EventBus eventBus) {
        eventBus.listen(PlayerInventoryEvent.Click.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerInventoryEvent.Click",
                    "slot: " + event.slot() + " | item: "
                        + event.itemStack().item().key());
            });

        eventBus.listen(PlayerInventoryEvent.Open.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerInventoryEvent.Open",
                    "player: " + event.player().name());
            });

        eventBus.listen(PlayerInventoryEvent.Close.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerInventoryEvent.Close",
                    "player: " + event.player().name());
            });

        eventBus.listen(PlayerInventoryEvent.HeldItemChange.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(), "PlayerInventoryEvent.HeldItemChange", "slot: " + event.slot());
            });
    }

    // ── Player game mode ───────────────────────────────────────────

    private static void registerPlayerGameModeEvent(EventBus eventBus) {
        eventBus.listen(PlayerGameModeChangeEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerGameModeChangeEvent", "mode: " + event.gameMode());
            });
    }

    // ── Player swap hand items ─────────────────────────────────────

    private static void registerPlayerSwapHandItemsEvent(EventBus eventBus) {
        eventBus.listen(PlayerSwapHandItemsEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerSwapHandItemsEvent",
                    "player: " + event.player().name());
            });
    }

    // ── Player advancement ─────────────────────────────────────────

    private static void registerPlayerAdvancementEvent(EventBus eventBus) {
        eventBus.listen(PlayerAdvancementEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerAdvancementEvent",
                    "advancement: " + event.advancement());
            });
    }

    // ── Player animation ───────────────────────────────────────────

    private static void registerPlayerAnimationEvent(EventBus eventBus) {
        eventBus.listen(PlayerAnimationEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerAnimationEvent", "type: " + event.animationType());
            });
    }

    // ── Player bed ─────────────────────────────────────────────────

    private static void registerPlayerBedEvents(EventBus eventBus) {
        eventBus.listen(PlayerBedEvent.Enter.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerBedEvent.Enter", "bed: " + event.bedLocation());
            });

        eventBus.listen(PlayerBedEvent.Leave.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerBedEvent.Leave", "bed: " + event.bedLocation());
            });
    }

    // ── Player experience ──────────────────────────────────────────

    private static void registerPlayerExperienceEvents(EventBus eventBus) {
        eventBus.listen(PlayerExperienceEvent.Change.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(), "PlayerExperienceEvent.Change", "amount: " + event.amount());
            });

        eventBus.listen(PlayerExperienceEvent.LevelChange.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerExperienceEvent.LevelChange",
                    "old: " + event.oldLevel() + " | new: " + event.newLevel());
            });
    }

    // ── Player input ───────────────────────────────────────────────

    private static void registerPlayerInputEvent(EventBus eventBus) {
        eventBus.listen(PlayerInputEvent.class).priority(EventPriority.MONITOR).handler(event -> {
            if (event.sprint() || event.jump()) {
                sendDebug(
                    event.player(),
                    "PlayerInputEvent",
                    "sprint: " + event.sprint() + " | jump: " + event.jump());
            }
        });
    }

    // ── Vehicle ────────────────────────────────────────────────────

    private static void registerVehicleEvents(EventBus eventBus) {
        eventBus.listen(VehicleEvent.Enter.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "VehicleEvent.Enter",
                    "vehicle: " + event.vehicle().name());
            });

        eventBus.listen(VehicleEvent.Exit.class).priority(EventPriority.MONITOR).handler(event -> {
            sendDebug(
                event.player(),
                "VehicleEvent.Exit",
                "vehicle: " + event.vehicle().name());
        });
    }

    // ── Block ──────────────────────────────────────────────────────

    private static void registerBlockEvents(EventBus eventBus) {
        eventBus.listen(BlockEvent.Break.class).priority(EventPriority.MONITOR).handler(event -> {
            sendDebug(
                event.player(),
                "BlockEvent.Break",
                "block: " + event.block().type() + " | pos: "
                    + formatVec(event.block().position()));
        });

        eventBus.listen(BlockEvent.Place.class).priority(EventPriority.MONITOR).handler(event -> {
            sendDebug(
                event.player(),
                "BlockEvent.Place",
                "block: " + event.block().type() + " | pos: "
                    + formatVec(event.block().position()));
        });
    }

    // ── Entity ─────────────────────────────────────────────────────

    private static void registerEntityEvents(EventBus eventBus) {
        eventBus.listen(EntityEvent.Spawn.class).priority(EventPriority.MONITOR).handler(event -> {
            String entityName = event.entity().name();
            if (entityName.contains("item")) {
                return;
            }
            broadcastDebug(
                "EntityEvent.Spawn",
                "entity: " + entityName + " | reason: " + event.reason() + " | loc: "
                    + event.location());
        });

        eventBus.listen(EntityEvent.Death.class).priority(EventPriority.MONITOR).handler(event -> {
            broadcastDebug("EntityEvent.Death", "entity: " + event.entity().name());
        });
    }

    // ── Inventory (crafting/enchanting) ────────────────────────────

    private static void registerInventoryEvents(EventBus eventBus) {
        eventBus.listen(InventoryEvent.Craft.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "InventoryEvent.Craft",
                    "result: " + event.result().item().key() + " x"
                        + event.result().count());
            });

        eventBus.listen(InventoryEvent.Enchant.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "InventoryEvent.Enchant",
                    "item: " + event.item().item().key() + " | level: " + event.level());
            });
    }

    // ── Command execute ────────────────────────────────────────────

    private static void registerCommandExecuteEvent(EventBus eventBus) {
        eventBus.listen(CommandExecuteEvent.Pre.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                event.sender().asPlayer().ifPresent(player -> {
                    sendDebug(player, "CommandExecuteEvent.Pre", "cmd: " + event.command());
                });
            });
    }

    // ── Player client brand ────────────────────────────────────────

    private static void registerPlayerClientBrandEvent(EventBus eventBus) {
        eventBus.listen(PlayerClientBrandEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(event.player(), "PlayerClientBrandEvent", "brand: " + event.brand());
            });
    }

    // ── Player settings changed ────────────────────────────────────

    private static void registerPlayerSettingsChangedEvent(EventBus eventBus) {
        eventBus.listen(PlayerSettingsChangedEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                sendDebug(
                    event.player(),
                    "PlayerSettingsChangedEvent",
                    "lang: " + event.language() + " | viewDist: " + event.viewDistance());
            });
    }

    // ── Helpers ────────────────────────────────────────────────────

    private static void sendDebug(Player player, String eventName, String data) {
        ALL_EVENT_NAMES.add(eventName);
        if (player == null || !isEnabled(player.id(), eventName)) {
            return;
        }
        player.sendMessage(formatMessage(eventName, data));
    }

    private static void broadcastDebug(String eventName, String data) {
        ALL_EVENT_NAMES.add(eventName);
        ChatComponent message = formatMessage(eventName, data);
        for (Player player : Axiom.players()) {
            if (isEnabled(player.id(), eventName)) {
                player.sendMessage(message);
            }
        }
    }

    // ── World events ────────────────────────────────────────────────

    private static void registerWorldEvents(EventBus eventBus) {
        eventBus.listen(WorldChunkEvent.PreLoad.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldChunkEvent.PreLoad",
                    "chunk: " + event.chunkX() + ", " + event.chunkZ() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldChunkEvent.PostLoad.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldChunkEvent.PostLoad",
                    "chunk: " + event.chunkX() + ", " + event.chunkZ() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldChunkEvent.PreUnload.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldChunkEvent.PreUnload",
                    "chunk: " + event.chunkX() + ", " + event.chunkZ() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldChunkEvent.PostUnload.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldChunkEvent.PostUnload",
                    "chunk: " + event.chunkX() + ", " + event.chunkZ() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldTimeSkipEvent.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldTimeSkipEvent",
                    "amount: " + event.skipAmount() + " | reason: " + event.reason() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldWeatherEvent.Change.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldWeatherEvent.Change",
                    "raining: " + event.raining() + " | world: " + event.world().name());
            });

        eventBus.listen(WorldWeatherEvent.ThunderChange.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldWeatherEvent.ThunderChange",
                    "thundering: " + event.thundering() + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldWeatherEvent.LightningStrike.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldWeatherEvent.LightningStrike",
                    "pos: " + formatVec(event.position()) + " | world: "
                        + event.world().name());
            });

        eventBus.listen(WorldLifecycleEvent.Save.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "WorldLifecycleEvent.Save", "world: " + event.world().name());
            });
    }

    // ── Redstone events ─────────────────────────────────────────────

    private static void registerRedstoneEvents(EventBus eventBus) {
        eventBus.listen(RedstoneEvent.PowerChange.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "RedstoneEvent.PowerChange",
                    "old: " + event.oldPower() + " | new: " + event.newPower());
            });

        eventBus.listen(RedstoneEvent.NoteBlockPlay.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "RedstoneEvent.NoteBlockPlay",
                    "note: " + event.note() + " | instrument: " + event.instrument());
            });

        eventBus.listen(RedstoneEvent.PistonExtend.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "RedstoneEvent.PistonExtend",
                    "affected: " + event.affectedBlocks().size() + " blocks");
            });

        eventBus.listen(RedstoneEvent.PistonRetract.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug(
                    "RedstoneEvent.PistonRetract",
                    "affected: " + event.affectedBlocks().size() + " blocks");
            });

        eventBus.listen(RedstoneEvent.ButtonPress.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug("RedstoneEvent.ButtonPress", "");
            });

        eventBus.listen(RedstoneEvent.LeverToggle.class)
            .priority(EventPriority.MONITOR)
            .handler(event -> {
                broadcastDebug("RedstoneEvent.LeverToggle", "powered: " + event.isPowered());
            });
    }

    // ── Helpers ────────────────────────────────────────────────────

    private static ChatComponent formatMessage(String eventName, String data) {
        return ChatComponent.text("[DEBUG] ")
            .color(ChatColor.GRAY)
            .append(ChatComponent.text(eventName).color(ChatColor.YELLOW))
            .append(ChatComponent.text(" | " + data).color(ChatColor.WHITE));
    }

    private static String formatVec(Vector3 vec) {
        return String.format("%.0f, %.0f, %.0f", vec.x(), vec.y(), vec.z());
    }
}
