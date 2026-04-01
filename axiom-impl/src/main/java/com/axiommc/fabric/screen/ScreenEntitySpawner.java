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
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
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
 */
public final class ScreenEntitySpawner {

    private static final AtomicInteger NEXT_ID = new AtomicInteger(Integer.MAX_VALUE);
    private static final FabricComponentSerializer SERIALIZER = new FabricComponentSerializer();

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
        // Push text/buttons in front of panels toward the player
        Vec3 frontOffset = forward.scale(-0.3);

        ServerLevel level = (ServerLevel) player.level();

        for (ScreenElement element : screen.elements()) {
            switch (element) {
                case ScreenElement.Panel panel -> {
                    entityIds.add(spawnPanel(player, level, panel, screen, center, right, up, yaw));
                }
                case ScreenElement.Label label -> {
                    entityIds.add(spawnLabel(player, level, label, screen, center.add(frontOffset), right, up, yaw));
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

        int cursorId = spawnCursor(player, level, center.add(frontOffset));
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
                new ClientboundRemoveEntitiesPacket(entityIds.stream().mapToInt(i -> i).toArray())
        );
    }

    // ── Element spawners ──────────────────────────────────────────────────────

    private static int spawnPanel(ServerPlayer player, ServerLevel level,
                                  ScreenElement.Panel panel, Screen screen,
                                  Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.BlockDisplay entity = new Display.BlockDisplay(EntityType.BLOCK_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        float cx = panel.x() + panel.width() / 2f;
        float cy = panel.y() + panel.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, cx, cy, screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        setBlockState(entity, panelStyleToBlock(panel.style()));

        float scaleX = panel.width() * screen.width();
        float scaleY = panel.height() * screen.height();
        setScale(entity, scaleX, scaleY, 0.01f);
        setTranslation(entity, -scaleX / 2f, -scaleY / 2f, 0f);
        setBillboard(entity, Display.BillboardConstraints.FIXED);

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

        setText(entity, SERIALIZER.serialize(label.text()));
        setTextBackground(entity, 0x00000000);
        setBillboard(entity, Display.BillboardConstraints.FIXED);

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

        setText(entity, SERIALIZER.serialize(button.label()));
        setTextBackground(entity, 0x40000000);
        setBillboard(entity, Display.BillboardConstraints.FIXED);

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
        setInteractionSize(entity, w, h);

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
        setInteractionSize(entity, size, size);

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
        setItemStack(entity, new ItemStack(item));
        float size = slot.size() * Math.min(screen.width(), screen.height());
        setScale(entity, size, size, size);
        setBillboard(entity, Display.BillboardConstraints.FIXED);

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnCursor(ServerPlayer player, ServerLevel level, Vec3 pos) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        int id = nextId();
        entity.setId(id);

        entity.setPos(pos.x, pos.y, pos.z);
        setText(entity, net.minecraft.network.chat.Component.literal("⬤").withColor(0xFFFFFF));
        setTextBackground(entity, 0x00000000);
        setScale(entity, 0.3f, 0.3f, 0.3f);
        setBillboard(entity, Display.BillboardConstraints.FIXED);

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

    private static BlockState panelStyleToBlock(PanelStyle style) {
        return switch (style) {
            case DARK   -> Blocks.SMOOTH_STONE.defaultBlockState();
            case GLASS  -> Blocks.TINTED_GLASS.defaultBlockState();
            case BORDER -> Blocks.BLACKSTONE.defaultBlockState();
            case ACCENT -> Blocks.WARPED_PLANKS.defaultBlockState();
        };
    }

    // ── Reflection helpers ────────────────────────────────────────────────────
    // Direct calls to private methods/fields are not possible without Loom/AccessWidener.
    // In 26.1 the names are stable and unobfuscated so these will not break on updates.

    private static void setBlockState(Display.BlockDisplay entity, BlockState state) {
        invokePrivate(entity, Display.BlockDisplay.class, "setBlockState", BlockState.class, state);
    }

    private static void setText(Display.TextDisplay entity, net.minecraft.network.chat.Component text) {
        invokePrivate(entity, Display.TextDisplay.class, "setText",
                net.minecraft.network.chat.Component.class, text);
    }

    private static void setItemStack(Display.ItemDisplay entity, ItemStack stack) {
        invokePrivate(entity, Display.ItemDisplay.class, "setItemStack", ItemStack.class, stack);
    }

    private static void setInteractionSize(Interaction entity, float width, float height) {
        invokePrivate(entity, Interaction.class, "setWidth",    float.class, width);
        invokePrivate(entity, Interaction.class, "setHeight",   float.class, height);
        invokePrivate(entity, Interaction.class, "setResponse", boolean.class, true);
    }

    private static void setTextBackground(Display.TextDisplay entity, int argb) {
        setEntityData(entity, Display.TextDisplay.class, "DATA_BACKGROUND_COLOR_ID", Integer.class, argb);
    }

    private static void setScale(Display entity, float x, float y, float z) {
        setEntityData(entity, Display.class, "DATA_SCALE_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(x, y, z));
    }

    private static void setTranslation(Display entity, float x, float y, float z) {
        setEntityData(entity, Display.class, "DATA_TRANSLATION_ID",
                org.joml.Vector3f.class, new org.joml.Vector3f(x, y, z));
    }

    private static void setBillboard(Display entity, Display.BillboardConstraints mode) {
        setEntityData(entity, Display.class, "DATA_BILLBOARD_RENDER_CONSTRAINTS_ID",
                Byte.class, (byte) mode.ordinal());
    }

    /** Sets a synced entity data field via its static EntityDataAccessor. */
    @SuppressWarnings({"unchecked", "rawtypes"})
    private static <T> void setEntityData(net.minecraft.world.entity.Entity entity,
                                          Class<?> holderClass, String fieldName,
                                          Class<T> type, T value) {
        try {
            var field = holderClass.getDeclaredField(fieldName);
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor) field.get(null);
            entity.getEntityData().set(accessor, value);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set entity data field {}: {}", fieldName, e.getMessage());
        }
    }

    /** Invokes a private instance method via reflection. */
    private static void invokePrivate(Object target, Class<?> declaringClass,
                                      String methodName, Class<?> paramType, Object value) {
        try {
            Method method = declaringClass.getDeclaredMethod(methodName, paramType);
            method.setAccessible(true);
            method.invoke(target, value);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to invoke {}.{}: {}", declaringClass.getSimpleName(), methodName, e.getMessage());
        }
    }

    private static void sendSpawnPackets(ServerPlayer player, net.minecraft.world.entity.Entity entity, int id) {
        player.connection.send(new ClientboundAddEntityPacket(entity, 0, entity.blockPosition()));
        var packedData = entity.getEntityData().getNonDefaultValues();
        if (packedData != null) {
            player.connection.send(new ClientboundSetEntityDataPacket(id, packedData));
        }
    }

    private static int nextId() {
        return NEXT_ID.getAndDecrement();
    }
}