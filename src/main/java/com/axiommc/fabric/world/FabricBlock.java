package com.axiommc.fabric.world;

import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.World;
import com.axiommc.api.world.block.Block;
import com.axiommc.api.world.block.BlockFace;
import com.axiommc.api.world.block.BlockState;
import com.axiommc.api.world.block.Material;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerLevel;

import java.util.HashMap;
import java.util.Map;

public class FabricBlock implements Block {

    private final ServerLevel serverLevel;
    private final BlockPos blockPos;
    private final FabricWorld world;

    public FabricBlock(ServerLevel serverLevel, int x, int y, int z, FabricWorld world) {
        this.serverLevel = serverLevel;
        this.blockPos = BlockPos.containing(x, y, z);
        this.world = world;
    }

    // ============================================================
    // Block Properties
    // ============================================================

    // ────────────────────────────────────────────────────────
    // Position & World
    // ────────────────────────────────────────────────────────

    @Override
    public Vector3 position() {
        return new Vector3(blockPos.getX(), blockPos.getY(), blockPos.getZ());
    }

    @Override
    public World world() {
        return world;
    }

    // ────────────────────────────────────────────────────────
    // Type & State
    // ────────────────────────────────────────────────────────

    @Override
    public Material type() {
        var mcBlock = serverLevel.getBlockState(blockPos).getBlock();
        var identifier = BuiltInRegistries.BLOCK.getKey(mcBlock);
        return Material.of(identifier.toString());
    }

    @Override
    public BlockState state() {
        var mcBlockState = serverLevel.getBlockState(blockPos);
        var properties = new HashMap<String, String>();

        for (var property : mcBlockState.getProperties()) {
            var value = mcBlockState.getValue(property);
            properties.put(property.getName(), value.toString());
        }

        return new BlockState(properties);
    }

    // ────────────────────────────────────────────────────────
    // Lighting
    // ────────────────────────────────────────────────────────

    @Override
    public int lightLevel() {
        return serverLevel.getBlockState(blockPos).getLightEmission();
    }

    // ────────────────────────────────────────────────────────
    // Material Properties
    // ────────────────────────────────────────────────────────

    @Override
    public boolean solid() {
        var blockState = serverLevel.getBlockState(blockPos);
        return !blockState.isAir();
    }

    @Override
    public boolean liquid() {
        return !serverLevel.getBlockState(blockPos).getFluidState().isEmpty();
    }

    @Override
    public boolean air() {
        return serverLevel.getBlockState(blockPos).isAir();
    }

    @Override
    public boolean opaque() {
        return !serverLevel.getBlockState(blockPos).isAir();
    }

    // ============================================================
    // Relative Block Access
    // ============================================================

    @Override
    public Block relative(BlockFace face) {
        var direction = switch (face) {
            case UP -> Direction.UP;
            case DOWN -> Direction.DOWN;
            case NORTH -> Direction.NORTH;
            case SOUTH -> Direction.SOUTH;
            case EAST -> Direction.EAST;
            case WEST -> Direction.WEST;
        };
        var newPos = blockPos.relative(direction);
        return new FabricBlock(serverLevel, newPos.getX(), newPos.getY(), newPos.getZ(), world);
    }

    @Override
    public Block relative(Vector3 delta) {
        var newX = (int) (blockPos.getX() + delta.x());
        var newY = (int) (blockPos.getY() + delta.y());
        var newZ = (int) (blockPos.getZ() + delta.z());
        return new FabricBlock(serverLevel, newX, newY, newZ, world);
    }

    // ============================================================
    // Block Modification
    // ============================================================

    @Override
    public void type(Material type) {
        try {
            var newBlockState = parseBlockState(type);
            if (newBlockState != null) {
                serverLevel.setBlock(blockPos, newBlockState, 3);
            }
        } catch (Exception e) {
            // Silently ignore if block lookup fails
        }
    }

    private net.minecraft.world.level.block.state.BlockState parseBlockState(Material type) {
        var identifier = Identifier.tryParse(type.id());
        if (identifier == null) return null;

        var blockHolder = BuiltInRegistries.BLOCK.get(identifier);
        if (blockHolder.isEmpty()) return null;

        var mcBlock = blockHolder.get().value();
        return mcBlock.defaultBlockState();
    }

    @Override
    public boolean breakNaturally() {
        return serverLevel.destroyBlock(blockPos, true);
    }
}
