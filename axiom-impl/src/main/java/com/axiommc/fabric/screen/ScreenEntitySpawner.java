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
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.entity.Display;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.phys.Vec3;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Spawns display entities for a Screen using packets (client-side only).
 * Entities are never added to the server world — only visible to the target player.
 */
public final class ScreenEntitySpawner {

    private static final AtomicInteger NEXT_ID = new AtomicInteger(Integer.MAX_VALUE / 2);
    private static final FabricComponentSerializer SERIALIZER = new FabricComponentSerializer();

    private ScreenEntitySpawner() {}

    /**
     * Spawns all display entities for a screen in front of the player.
     *
     * @return list of spawned entity IDs (for later despawning)
     */
    public static List<Integer> spawnScreen(ServerPlayer player, Screen screen) {
        List<Integer> entityIds = new ArrayList<>();

        Vec3 eyePos = player.getEyePosition();
        Vec3 forward = player.getLookAngle().normalize();
        Vec3 right = forward.cross(new Vec3(0, 1, 0)).normalize();
        Vec3 up = right.cross(forward).normalize();
        Vec3 center = eyePos.add(forward.scale(screen.distance()));

        float playerYaw = player.getYRot();

        // Text/button/item entities sit slightly in front of panels
        Vec3 frontOffset = forward.scale(-0.15);

        for (ScreenElement element : screen.elements()) {
            switch (element) {
                case ScreenElement.Panel panel ->
                    entityIds.add(spawnPanel(player, panel, screen, center, right, up, playerYaw));
                case ScreenElement.Label label ->
                    entityIds.add(spawnLabel(player, label, screen, center.add(frontOffset), right, up, playerYaw));
                case ScreenElement.Button button ->
                    entityIds.add(spawnButton(player, button, screen, center.add(frontOffset), right, up, playerYaw));
                case ScreenElement.ItemSlot itemSlot ->
                    entityIds.add(spawnItemSlot(player, itemSlot, screen, center.add(frontOffset), right, up, playerYaw));
            }
        }

        return entityIds;
    }

    /**
     * Removes all display entities by sending a remove packet to the player.
     */
    public static void despawnEntities(ServerPlayer player, List<Integer> entityIds) {
        if (entityIds.isEmpty()) {
            return;
        }
        player.connection.send(new ClientboundRemoveEntitiesPacket(entityIds.stream().mapToInt(i -> i).toArray()));
    }

    // ── Element Spawners ─────────────────────────────────────────────────────

    private static int spawnPanel(ServerPlayer player, ScreenElement.Panel panel,
                                  Screen screen, Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.BlockDisplay entity = new Display.BlockDisplay(EntityType.BLOCK_DISPLAY, (net.minecraft.server.level.ServerLevel) player.level());
        int id = NEXT_ID.getAndDecrement();
        entity.setId(id);

        float centerX = panel.x() + panel.width() / 2f;
        float centerY = panel.y() + panel.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, centerX, centerY, screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        // Set block state via reflection (private method)
        invokePrivate(entity, "setBlockState", BlockState.class, panelStyleToBlock(panel.style()));

        // Scale to flat panel: width/height in world units, paper-thin Z
        float scaleX = panel.width() * screen.width();
        float scaleY = panel.height() * screen.height();
        setScale(entity, scaleX, scaleY, 0.01f);
        // Center the panel on its position (default origin is corner)
        setTranslation(entity, -scaleX / 2f, -scaleY / 2f, 0f);
        setBillboard(entity, (byte) 0); // FIXED

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnLabel(ServerPlayer player, ScreenElement.Label label,
                                  Screen screen, Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, (net.minecraft.server.level.ServerLevel) player.level());
        int id = NEXT_ID.getAndDecrement();
        entity.setId(id);

        Vec3 pos = elementPosition(center, right, up, label.x(), label.y(), screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        setTextDisplayText(entity, SERIALIZER.serialize(label.text()));
        setTextDisplayBackground(entity, 0);
        setBillboard(entity, (byte) 0); // FIXED

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnButton(ServerPlayer player, ScreenElement.Button button,
                                   Screen screen, Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, (net.minecraft.server.level.ServerLevel) player.level());
        int id = NEXT_ID.getAndDecrement();
        entity.setId(id);

        float centerX = button.x() + button.width() / 2f;
        float centerY = button.y() + button.height() / 2f;
        Vec3 pos = elementPosition(center, right, up, centerX, centerY, screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        setTextDisplayText(entity, SERIALIZER.serialize(button.label()));
        setTextDisplayBackground(entity, 0x40000000);
        setBillboard(entity, (byte) 0); // FIXED

        sendSpawnPackets(player, entity, id);
        return id;
    }

    private static int spawnItemSlot(ServerPlayer player, ScreenElement.ItemSlot itemSlot,
                                     Screen screen, Vec3 center, Vec3 right, Vec3 up, float yaw) {
        Display.ItemDisplay entity = new Display.ItemDisplay(EntityType.ITEM_DISPLAY, (net.minecraft.server.level.ServerLevel) player.level());
        int id = NEXT_ID.getAndDecrement();
        entity.setId(id);

        Vec3 pos = elementPosition(center, right, up, itemSlot.x(), itemSlot.y(), screen);
        entity.setPos(pos.x, pos.y, pos.z);
        entity.setYRot(yaw + 180f);

        var item = BuiltInRegistries.ITEM.getValue(Identifier.parse(itemSlot.item()));
        invokePrivate(entity, "setItemStack", ItemStack.class, new ItemStack(item));
        setScale(entity, itemSlot.size(), itemSlot.size(), itemSlot.size());
        setBillboard(entity, (byte) 0); // FIXED

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
            case DARK -> Blocks.SMOOTH_STONE.defaultBlockState();
            case GLASS -> Blocks.TINTED_GLASS.defaultBlockState();
            case BORDER -> Blocks.BLACKSTONE.defaultBlockState();
            case ACCENT -> Blocks.WARPED_PLANKS.defaultBlockState();
        };
    }

    @SuppressWarnings("unchecked")
    private static void setTextDisplayText(Display.TextDisplay entity, net.minecraft.network.chat.Component text) {
        try {
            var field = Display.TextDisplay.class.getDeclaredField("DATA_TEXT_ID");
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor<net.minecraft.network.chat.Component>) field.get(null);
            entity.getEntityData().set(accessor, text);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set text display text", e);
        }
    }

    @SuppressWarnings("unchecked")
    private static void setTextDisplayBackground(Display.TextDisplay entity, int color) {
        try {
            var field = Display.TextDisplay.class.getDeclaredField("DATA_BACKGROUND_COLOR_ID");
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor<Integer>) field.get(null);
            entity.getEntityData().set(accessor, color);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set text display background", e);
        }
    }

    @SuppressWarnings("unchecked")
    private static void setScale(Display entity, float x, float y, float z) {
        try {
            var field = Display.class.getDeclaredField("DATA_SCALE_ID");
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor<org.joml.Vector3fc>) field.get(null);
            entity.getEntityData().set(accessor, new org.joml.Vector3f(x, y, z));
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set scale", e);
        }
    }

    @SuppressWarnings("unchecked")
    private static void setTranslation(Display entity, float x, float y, float z) {
        try {
            var field = Display.class.getDeclaredField("DATA_TRANSLATION_ID");
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor<org.joml.Vector3fc>) field.get(null);
            entity.getEntityData().set(accessor, new org.joml.Vector3f(x, y, z));
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set translation", e);
        }
    }

    @SuppressWarnings("unchecked")
    private static void setBillboard(Display entity, byte mode) {
        try {
            var field = Display.class.getDeclaredField("DATA_BILLBOARD_RENDER_CONSTRAINTS_ID");
            field.setAccessible(true);
            var accessor = (net.minecraft.network.syncher.EntityDataAccessor<Byte>) field.get(null);
            entity.getEntityData().set(accessor, mode);
        } catch (Exception e) {
            Axiom.logger().debug("Failed to set billboard", e);
        }
    }

    private static void sendSpawnPackets(ServerPlayer player, Display entity, int entityId) {
        player.connection.send(new ClientboundAddEntityPacket(entity, 0, entity.blockPosition()));
        var packedData = entity.getEntityData().getNonDefaultValues();
        if (packedData != null) {
            player.connection.send(new ClientboundSetEntityDataPacket(entityId, packedData));
        }
    }

    /**
     * Invokes a private method on an entity via reflection.
     */
    private static void invokePrivate(Object target, String methodName, Class<?> paramType, Object value) {
        try {
            Method method = findMethod(target.getClass(), methodName, paramType);
            if (method != null) {
                method.setAccessible(true);
                method.invoke(target, value);
            }
        } catch (Exception e) {
            Axiom.logger().debug("Failed to invoke {}.{}", target.getClass().getSimpleName(), methodName, e);
        }
    }

    private static Method findMethod(Class<?> clazz, String name, Class<?> paramType) {
        while (clazz != null) {
            try {
                return clazz.getDeclaredMethod(name, paramType);
            } catch (NoSuchMethodException e) {
                clazz = clazz.getSuperclass();
            }
        }
        return null;
    }
}
