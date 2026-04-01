package com.axiommc.fabric.screen;

import com.axiommc.api.screen.PanelStyle;
import com.axiommc.api.screen.Screen;
import com.axiommc.api.screen.ScreenElement;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.network.protocol.game.ClientboundAddEntityPacket;
import net.minecraft.network.protocol.game.ClientboundRemoveEntitiesPacket;
import net.minecraft.network.protocol.game.ClientboundSetEntityDataPacket;
import net.minecraft.network.protocol.game.ClientboundTeleportEntityPacket;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.entity.Display;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.Interaction;
import net.minecraft.world.entity.PositionMoveRotation;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.phys.Vec3;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Spawns display and interaction entities for a Screen using packets (client-side only).
 * Entities are never added to the server world — only visible to the target player.
 *
 * <p>Panels use TextDisplay with a solid backgroundColor — no block textures.
 * Full brightness (15, 15) applied to all entities so world lighting has no effect.
 *
 * <p>Field/method names verified against 26.1 unobfuscated Mojang mappings and
 * cross-referenced with CursorCS decompiled source.
 */
public final class ScreenEntitySpawner {

    // Entity IDs count down from MAX_VALUE to stay far from real server IDs (which grow up from 1)
    private static final AtomicInteger NEXT_ID = new AtomicInteger(Integer.MAX_VALUE);
    private static final FabricComponentSerializer SERIALIZER = new FabricComponentSerializer();

    // Billboard mode — matches Display.BillboardConstraints ordinal in 26.1
    private static final byte BILLBOARD_FIXED = 0;

    private ScreenEntitySpawner() {}

    // ── Spawn result ──────────────────────────────────────────────────────────

    public record SpawnResult(
            List<Integer> entityIds,
            Map<Integer, ScreenElement> interactionMap,
            int cursorEntityId
    ) {}

    // ── Public API ────────────────────────────────────────────────────────────

    public static SpawnResult spawnScreen(ServerPlayer player, Screen screen) {
        List<Integer> entityIds = new ArrayList<>();
        Map<Integer, ScreenElement> interactionMap = new HashMap<>();

        Vec3 eyePos  = player.getEyePosition();
        Vec3 forward = player.getLookAngle().normalize();
        Vec3 right   = forward.cross(new Vec3(0, 1, 0)).normalize();
        Vec3 up      = right.cross(forward).normalize();
        Vec3 center  = eyePos.add(forward.scale(screen.distance()));

        float yaw = player.getYRot();
        // Labels/buttons/items sit slightly in front of panels to avoid z-fighting
        Vec3 frontOffset = forward.scale(-0.05);

        ServerLevel level = (ServerLevel) player.level();

        for (ScreenElement element : screen.elements()) {
            switch (element) {
                case ScreenElement.Panel panel -> {
                    entityIds.add(spawnPanel(player, level, panel, screen, center, right, up, yaw));
                }
                case ScreenElement.Label label -> {
                    entityIds.add(spawnLabel(player, level, label, screen,
                            center.add(frontOffset), right, up, yaw));
                }
                case ScreenElement.Button button -> {
                    Vec3 bc = center.add(frontOffset);
                    entityIds.add(spawnButton(player, level, button, screen, bc, right, up, yaw));
                    int interactionId = spawnInteraction(player, level, button, screen, bc, right, up);
                    entityIds.add(interactionId);
                    interactionMap.put(interactionId, button);
                }
                case ScreenElement.ItemSlot slot -> {
                    Vec3 sc = center.add(frontOffset);
                    entityIds.add(spawnItemSlot(player, level, slot, screen, sc, right, up, yaw));
                    if (slot.onClick() != null) {
                        int interactionId = spawnInteractionForItem(player, level, slot, screen, sc, right, up);
                        entityIds.add(interactionId);
                        interactionMap.put(interactionId, slot);
                    }
                }
            }
        }

        // Cursor spawned last — renders on top of everything
        int cursorId = spawnCursor(player, level, center.add(frontOffset.scale(2)));
        entityIds.add(cursorId);

        return new SpawnResult(entityIds, interactionMap, cursorId);
    }

    public static void moveCursor(ServerPlayer player, int cursorEntityId, Vec3 pos) {
        player.connection.send(ClientboundTeleportEntityPacket.teleport(
                cursorEntityId,
                new PositionMoveRotation(pos, Vec3.ZERO, 0f, 0f),
                Set.of(),
                false
        ));
    }

    public static void despawnEntities(ServerPlayer player, List<Integer> entityIds) {
        if (entityIds.isEmpty()) return;
        player.connection.send(
                new ClientboundRemoveEntitiesPacket(entityIds.stream().mapToInt(i -> i).toArray()));
    }

    // ── Element spawners ──────────────────────────────────────────────────────

    /**
     * Panel = TextDisplay with solid backgroundColor, empty text, full brightness.
     * Renders as a clean flat colored rectangle — no block textures.
     */
    private static int spawnPanel(ServerPlayer player, ServerLevel level,
                                  ScreenElement.Panel panel, Screen screen,
                                  Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        float cx = panel.x() + panel.width() / 2f;
        float cy = panel.y() + panel.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, cx, cy, screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        int argb = panelStyleToArgb(panel.style());

        // Build a block of spaces with newlines to create a filled rectangle
        // The background color renders behind this text area
        int lineWidth = 200;
        int lineCount = 20;
        String spaceLine = " ".repeat(lineWidth);
        String fill = (spaceLine + "\n").repeat(lineCount);

        invokeMethod(entity, Display.TextDisplay.class, "setText",
                net.minecraft.network.chat.Component.class,
                net.minecraft.network.chat.Component.literal(fill));

        setData(entity, Display.TextDisplay.class, "DATA_BACKGROUND_COLOR_ID",
                Integer.class, argb);
        setData(entity, Display.TextDisplay.class, "DATA_LINE_WIDTH_ID",
                Integer.class, 10000);
        setData(entity, Display.TextDisplay.class, "DATA_TEXT_OPACITY_ID",
                Byte.class, (byte) -1); // fully opaque

        float worldWidth = panel.width() * screen.width();
        float worldHeight = panel.height() * screen.height();
        setData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(worldWidth, worldHeight, 1f));
        setData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, BILLBOARD_FIXED);
        setData(entity, Display.class, "DATA_BRIGHTNESS_OVERRIDE_ID",
                Integer.class, packBrightness(15, 15));

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnLabel(ServerPlayer player, ServerLevel level,
                                  ScreenElement.Label label, Screen screen,
                                  Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        Vec3 pos = elementPosition(center, right, up, label.x(), label.y(), screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        invokeMethod(entity, Display.TextDisplay.class, "setText",
                net.minecraft.network.chat.Component.class,
                SERIALIZER.serialize(label.text()));

        setData(entity, Display.TextDisplay.class, "DATA_BACKGROUND_COLOR_ID",
                Integer.class, 0x00000000); // fully transparent background
        setData(entity, Display.TextDisplay.class, "DATA_LINE_WIDTH_ID",
                Integer.class, 10000);
        setData(entity, Display.TextDisplay.class, "DATA_TEXT_OPACITY_ID",
                Byte.class, (byte) -1);
        setData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(0.5f, 0.5f, 0.5f));
        setData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, BILLBOARD_FIXED);
        setData(entity, Display.class, "DATA_BRIGHTNESS_OVERRIDE_ID",
                Integer.class, packBrightness(15, 15));

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnButton(ServerPlayer player, ServerLevel level,
                                   ScreenElement.Button button, Screen screen,
                                   Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        float cx = button.x() + button.width() / 2f;
        float cy = button.y() + button.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, cx, cy, screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        invokeMethod(entity, Display.TextDisplay.class, "setText",
                net.minecraft.network.chat.Component.class,
                SERIALIZER.serialize(button.label()));

        setData(entity, Display.TextDisplay.class, "DATA_BACKGROUND_COLOR_ID",
                Integer.class, 0xCC1E293B); // dark button background
        setData(entity, Display.TextDisplay.class, "DATA_LINE_WIDTH_ID",
                Integer.class, 10000);
        setData(entity, Display.TextDisplay.class, "DATA_TEXT_OPACITY_ID",
                Byte.class, (byte) -1);
        setData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(0.5f, 0.5f, 0.5f));
        setData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, BILLBOARD_FIXED);
        setData(entity, Display.class, "DATA_BRIGHTNESS_OVERRIDE_ID",
                Integer.class, packBrightness(15, 15));

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnInteraction(ServerPlayer player, ServerLevel level,
                                        ScreenElement.Button button, Screen screen,
                                        Vec3 center, Vec3 right, Vec3 up) {
        Interaction entity = new Interaction(EntityType.INTERACTION, level);
        int id = nextId();
        entity.setId(id);

        float cx = button.x() + button.width() / 2f;
        float cy = button.y() + button.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, cx, cy, screen);
        entity.setPos(pos.x, pos.y, pos.z);

        float w = button.width() * screen.width();
        float h = button.height() * screen.height();
        setData(entity, Interaction.class, "DATA_WIDTH_ID",  Float.class, w);
        setData(entity, Interaction.class, "DATA_HEIGHT_ID", Float.class, h);
        setData(entity, Interaction.class, "DATA_RESPONSE_ID", Boolean.class, true);

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnInteractionForItem(ServerPlayer player, ServerLevel level,
                                               ScreenElement.ItemSlot slot, Screen screen,
                                               Vec3 center, Vec3 right, Vec3 up) {
        Interaction entity = new Interaction(EntityType.INTERACTION, level);
        int id = nextId();
        entity.setId(id);

        Vec3 pos = elementPosition(center, right, up, slot.x(), slot.y(), screen);
        entity.setPos(pos.x, pos.y, pos.z);

        float size = slot.size() * Math.min(screen.width(), screen.height());
        setData(entity, Interaction.class, "DATA_WIDTH_ID",  Float.class, size);
        setData(entity, Interaction.class, "DATA_HEIGHT_ID", Float.class, size);
        setData(entity, Interaction.class, "DATA_RESPONSE_ID", Boolean.class, true);

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnItemSlot(ServerPlayer player, ServerLevel level,
                                     ScreenElement.ItemSlot slot, Screen screen,
                                     Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.ItemDisplay entity = new Display.ItemDisplay(EntityType.ITEM_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        Vec3 pos = elementPosition(center, right, up, slot.x(), slot.y(), screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        var item = BuiltInRegistries.ITEM.getValue(Identifier.parse(slot.item()));
        invokeMethod(entity, Display.ItemDisplay.class, "setItemStack",
                ItemStack.class, new ItemStack(item));

        float size = slot.size() * Math.min(screen.width(), screen.height());
        setData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(size, size, size));
        setData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, BILLBOARD_FIXED);
        setData(entity, Display.class, "DATA_BRIGHTNESS_OVERRIDE_ID",
                Integer.class, packBrightness(15, 15));

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnCursor(ServerPlayer player, ServerLevel level, Vec3 pos) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        entity.setPos(pos.x, pos.y, pos.z);

        invokeMethod(entity, Display.TextDisplay.class, "setText",
                net.minecraft.network.chat.Component.class,
                net.minecraft.network.chat.Component.literal("⬤").withColor(0xFFFFFF));

        setData(entity, Display.TextDisplay.class, "DATA_BACKGROUND_COLOR_ID",
                Integer.class, 0x00000000);
        setData(entity, Display.TextDisplay.class, "DATA_LINE_WIDTH_ID",
                Integer.class, 10000);
        setData(entity, Display.TextDisplay.class, "DATA_TEXT_OPACITY_ID",
                Byte.class, (byte) -1);
        setData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(0.15f, 0.15f, 0.15f));
        setData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, BILLBOARD_FIXED);
        setData(entity, Display.class, "DATA_BRIGHTNESS_OVERRIDE_ID",
                Integer.class, packBrightness(15, 15));

        sendSpawnPackets(player, entity, id);
        return id;
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static Vec3 elementPosition(Vec3 center, Vec3 right, Vec3 up,
                                        float normX, float normY, Screen screen) {
        double offsetX = (normX - 0.5) * screen.width();
        double offsetY = (0.5 - normY) * screen.height();
        return center.add(right.scale(offsetX)).add(up.scale(offsetY));
    }

    private static int panelStyleToArgb(PanelStyle style) {
        return switch (style) {
            case DARK   -> 0xEE0F172A; // near-black navy, mostly opaque
            case GLASS  -> 0x991E293B; // semi-transparent slate
            case BORDER -> 0xFF000000; // solid black
            case ACCENT -> 0xFF0E7490; // teal
        };
    }

    /**
     * Packs block and sky brightness into the format expected by DATA_BRIGHTNESS_OVERRIDE_ID.
     * Matches vanilla light level packing: (sky << 20) | (block << 4).
     * Values 0-15. Use packBrightness(15, 15) for maximum brightness.
     */
    private static int packBrightness(int block, int sky) {
        return (sky << 20) | (block << 4);
    }

    // ── Reflection ────────────────────────────────────────────────────────────
    // Cannot call private methods/access private fields directly without AccessWidener.
    // Field names are stable in 26.1 (fully unobfuscated Mojang mappings).

    /**
     * Sets a value on an EntityDataAccessor field via reflection.
     * Used for: DATA_BACKGROUND_COLOR_ID, DATA_LINE_WIDTH_ID, DATA_TEXT_OPACITY_ID,
     *           DATA_SCALE_ID, DATA_TRANSLATION_ID, DATA_BILLBOARD_RENDER_CONSTRAINTS_ID,
     *           DATA_BRIGHTNESS_OVERRIDE_ID, DATA_WIDTH_ID, DATA_HEIGHT_ID, DATA_RESPONSE_ID.
     */
    @SuppressWarnings({"unchecked", "rawtypes"})
    private static <T> void setData(net.minecraft.world.entity.Entity entity,
                                    Class<?> holderClass, String fieldName,
                                    Class<T> type, T value) {
        try {
            var field = holderClass.getDeclaredField(fieldName);
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor) field.get(null);
            entity.getEntityData().set(accessor, value);
        } catch (Exception e) {
            Axiom.logger().debug("setData({}.{}) failed: {}", holderClass.getSimpleName(), fieldName, e.getMessage());
        }
    }

    /**
     * Invokes a private method via reflection.
     * Used for: setText, setItemStack (private in 26.1 without AccessWidener).
     */
    private static void invokeMethod(Object target, Class<?> declaringClass,
                                     String methodName, Class<?> paramType, Object value) {
        try {
            Method method = declaringClass.getDeclaredMethod(methodName, paramType);
            method.setAccessible(true);
            method.invoke(target, value);
        } catch (Exception e) {
            Axiom.logger().debug("invokeMethod({}.{}) failed: {}",
                    declaringClass.getSimpleName(), methodName, e.getMessage());
        }
    }

    /**
     * Sends spawn + entity data packets to the player.
     * Uses packAll() (not getNonDefaultValues()) to ensure all configured
     * data fields are included — matches CursorCS approach.
     */
    private static void sendSpawnPackets(ServerPlayer player,
                                         net.minecraft.world.entity.Entity entity, int id) {
        player.connection.send(new ClientboundAddEntityPacket(entity, 0, entity.blockPosition()));
        try {
            // packAll() returns all synced data, not just dirty values — safer for initial spawn
            Method packAll = entity.getEntityData().getClass().getMethod("packAll");
            @SuppressWarnings("unchecked")
            List<?> packed = (List<?>) packAll.invoke(entity.getEntityData());
            if (packed != null && !packed.isEmpty()) {
                player.connection.send(new ClientboundSetEntityDataPacket(id, (List) packed));
            }
        } catch (Exception e) {
            // Fallback to getNonDefaultValues if packAll is renamed/unavailable
            var packed = entity.getEntityData().getNonDefaultValues();
            if (packed != null) {
                player.connection.send(new ClientboundSetEntityDataPacket(id, packed));
            }
        }
    }

    private static int nextId() {
        return NEXT_ID.getAndDecrement();
    }
}
