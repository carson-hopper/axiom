package com.axiommc.fabric.world;

import com.axiommc.api.entity.display.BlockDisplayEntity;
import com.axiommc.api.entity.display.BlockDisplaySpec;
import com.axiommc.api.entity.display.ItemDisplayEntity;
import com.axiommc.api.entity.display.ItemDisplaySpec;
import com.axiommc.api.entity.display.TextDisplayEntity;
import com.axiommc.api.entity.display.TextDisplaySpec;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.particle.ParticleEffect;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.Biome;
import com.axiommc.api.world.Chunk;
import com.axiommc.api.world.Difficulty;
import com.axiommc.api.world.Dimension;
import com.axiommc.api.world.DimensionType;
import com.axiommc.api.world.Weather;
import com.axiommc.api.world.World;
import com.axiommc.api.world.block.Block;
import com.axiommc.api.world.block.Material;
import com.axiommc.fabric.particle.FabricParticleConverter;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.util.TaskScheduler;
import net.minecraft.core.BlockPos;
import net.minecraft.core.particles.ParticleOptions;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.levelgen.Heightmap;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collection;
import java.util.Collections;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.stream.Collectors;

public record FabricWorld(ServerLevel level) implements World {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricWorld.class);

    // ============================================================
    // Identity & Metadata
    // ============================================================

    @Override
    public String name() {
        var dim = level.dimension();
        if (dim == Level.OVERWORLD) {
            return "minecraft:overworld";
        } else if (dim == Level.NETHER) {
            return "minecraft:the_nether";
        } else if (dim == Level.END) {
            return "minecraft:the_end";
        }
        return "minecraft:custom";
    }

    @Override
    public long seed() {
        return level.getSeed();
    }

    // ============================================================
    // Dimension & Environment
    // ============================================================

    @Override
    public Dimension dimension() {
        var dimType = mapDimensionType(level.dimension());

        return new Dimension(
            name(),
            dimType,
            dimType == DimensionType.OVERWORLD,
            dimType == DimensionType.NETHER,
            false, // hasFixedTime
                level.getMinY(),     // minY - approximation
                level.getMaxY(),   // height - approximation
            256,   // logicalHeight - approximation
            0.0f,  // ambientLight
            1.0d   // coordinateScale
        );
    }

    // ---- Time Management ----

    @Override
    public long time() {
        return level.getDefaultClockTime() % 24000;
    }

    @Override
    public void time(long time) {
        LOGGER.warn("Setting time directly is not supported in Minecraft 26.1");
    }

    @Override
    public long fullTime() {
        return level.getLevelData().getGameTime();
    }

    @Override
    public void fullTime(long time) {
        // Cannot directly set game time in Minecraft 26.1
        LOGGER.warn("Setting full time is not directly supported in Minecraft 26.1");
    }

    @Override
    public boolean dayTime() {
        long time = time();
        return time >= 0 && time < 12000;
    }

    // ---- Weather ----

    @Override
    public Weather weather() {
        if (level.isThundering()) {
            return Weather.THUNDER;
        } else if (level.isRaining()) {
            return Weather.RAIN;
        } else {
            return Weather.CLEAR;
        }
    }

    @Override
    public void weather(Weather weather) {
        switch (weather) {
            case CLEAR:
                level.setRainLevel(0f);
                level.setThunderLevel(0f);
                break;
            case RAIN:
                level.setRainLevel(1f);
                level.setThunderLevel(0f);
                break;
            case THUNDER:
                level.setRainLevel(1f);
                level.setThunderLevel(1f);
                break;
        }
    }

    // ---- Difficulty & PvP ----

    @Override
    public Difficulty difficulty() {
        return switch (level.getLevelData().getDifficulty()) {
            case PEACEFUL -> Difficulty.PEACEFUL;
            case EASY -> Difficulty.EASY;
            case NORMAL -> Difficulty.NORMAL;
            case HARD -> Difficulty.HARD;
        };
    }

    @Override
    public void difficulty(Difficulty difficulty) {
        // Cannot directly set difficulty in Minecraft 26.1
        LOGGER.warn("Setting difficulty is not directly supported in Minecraft 26.1");
    }

    @Override
    public boolean pvpEnabled() {
        return true; // Default to true; actual value not easily accessible in 26.1
    }

    // ============================================================
    // Spawn & Players
    // ============================================================

    @Override
    public Location spawnLocation() {
        var respawnData = level.getLevelData().getRespawnData();

        var spawnPos = respawnData.pos();
        var position = new Vector3(spawnPos.getX() + 0.5, spawnPos.getY() + 1.0, spawnPos.getZ() + 0.5);
        var rotation = new Vector2(respawnData.yaw(), respawnData.pitch());
        return new Location(this, position, rotation);
    }

    @Override
    public int playerCount() {
        return level.players().size();
    }

    @Override
    public Collection<? extends Player> players() {
        return level.players().stream()
                .filter(player -> !player.isRemoved())
                .map(FabricPlayer::new).toList();
    }

    // ============================================================
    // Chunks & Blocks
    // ============================================================

    @Override
    public Optional<Chunk> chunkAt(int chunkX, int chunkZ) {
        if (level.getChunkSource().hasChunk(chunkX, chunkZ)) {
            var chunk = level.getChunk(chunkX, chunkZ);
            return Optional.of(new FabricChunk(chunk, this));
        }
        return Optional.empty();
    }

    @Override
    public Chunk loadChunk(int chunkX, int chunkZ) {
        var chunk = level.getChunk(chunkX, chunkZ);
        return new FabricChunk(chunk, this);
    }

    @Override
    public CompletableFuture<Chunk> loadChunkAsync(int chunkX, int chunkZ) {
        var future = new CompletableFuture<Chunk>();
        // Minecraft 26.1 doesn't have a good async chunk loading API at this level
        // Fall back to sync loading on the server thread
        try {
            var chunk = loadChunk(chunkX, chunkZ);
            future.complete(chunk);
        } catch (Exception e) {
            future.completeExceptionally(e);
        }
        return future;
    }

    @Override
    public Block blockAt(int x, int y, int z) {
        int chunkX = toChunkCoord(x);
        int chunkZ = toChunkCoord(z);

        Chunk chunk = chunkAt(chunkX, chunkZ).orElse(null);
        if (chunk == null) {
            loadChunk(chunkX, chunkZ);
        }

        return new FabricBlock(level, x, y, z, this);
    }

    @Override
    public Optional<Biome> biomeAt(int x, int y, int z) {
        BlockPos blockPos = BlockPos.containing(x, y, z);
        try {
            var holder = level.getBiome(blockPos);
            var keyOpt = holder.unwrapKey();
            if (keyOpt.isPresent()) {
                var resourceKey = keyOpt.get();
                var identifier = resourceKey.identifier();
                var biomeId = identifier.getNamespace() + ":" + identifier.getPath();
                return Optional.of(Biome.of(biomeId));
            }
        } catch (Exception _) {
        }
        return Optional.empty();
    }

    @Override
    public int highestBlockY(int x, int z) {
        return level.getHeight(Heightmap.Types.WORLD_SURFACE, x, z);
    }

    @Override
    public void setBlock(int x, int y, int z, Material type) {
        try {
            BlockPos blockPos = BlockPos.containing(x, y, z);
            var identifier = net.minecraft.resources.Identifier.tryParse(type.id());
            if (identifier != null) {
                var blockHolder = BuiltInRegistries.BLOCK.get(identifier);
                if (blockHolder.isPresent()) {
                    var mcBlock = blockHolder.get().value();
                    var newBlockState = mcBlock.defaultBlockState();
                    level.setBlock(blockPos, newBlockState, 3);
                }
            }
        } catch (Exception e) {
            // Silently ignore if block lookup fails
        }
    }

    // ============================================================
    // Particles & Display Entities
    // ============================================================

    @Override
    public void spawnParticle(ParticleEffect effect, Location location) {
        ParticleOptions particleOptions = FabricParticleConverter.toMinecraftParticle(effect);
        if (particleOptions == null) {
            LOGGER.warn("Failed to convert particle effect: {}", effect.type().key());
            return;
        }

        Vector3 position = location.position();
        level.sendParticles(
            particleOptions,
            effect.force(),
            false,
            position.x(), position.y(), position.z(),
            effect.count(),
            effect.spreadX(),
            effect.spreadY(),
            effect.spreadZ(),
            effect.speed()
        );
    }

    @Override
    public void playSound(SoundKey sound, float volume, float pitch) {
        var loc = Identifier.tryParse(sound.key());
        if (loc == null) {
            LOGGER.warn("Invalid sound key: {}", sound.key());
            return;
        }

        var holder = BuiltInRegistries.SOUND_EVENT
            .getOptional(loc)
            .map(net.minecraft.core.Holder::direct)
            .orElse(net.minecraft.core.Holder.direct(net.minecraft.sounds.SoundEvent.createVariableRangeEvent(loc)));

        var spawn = spawnLocation();
        level.playSeededSound(
            null,
            spawn.position().x(),
            spawn.position().y(),
            spawn.position().z(),
            holder,
            net.minecraft.sounds.SoundSource.MASTER,
            volume, pitch,
            level.getRandom().nextLong()
        );
    }

    @Override
    public TextDisplayEntity spawnTextDisplay(TextDisplaySpec spec, Location location) {
        net.minecraft.world.entity.Display.TextDisplay entity =
            new net.minecraft.world.entity.Display.TextDisplay(net.minecraft.world.entity.EntityType.TEXT_DISPLAY, level);

        Vector3 pos = location.position();
        entity.setPos(pos.x(), pos.y(), pos.z());
        entity.setYRot(location.rotation().yaw());
        entity.setXRot(location.rotation().pitch());

        com.axiommc.fabric.entity.display.DisplayEntityUtil.applyTextDisplaySpec(entity, spec);
        level.addFreshEntity(entity);

        if (spec.ttl() > 0) {
            TaskScheduler.global().scheduleTask(spec.ttl(), () -> {
                if (!entity.isRemoved()) {
                    entity.discard();
                }
            });
        }

        return new com.axiommc.fabric.entity.display.FabricTextDisplayEntity(entity);
    }

    @Override
    public ItemDisplayEntity spawnItemDisplay(ItemDisplaySpec spec, Location location) {
        net.minecraft.world.entity.Display.ItemDisplay entity =
            new net.minecraft.world.entity.Display.ItemDisplay(net.minecraft.world.entity.EntityType.ITEM_DISPLAY, level);

        Vector3 pos = location.position();
        entity.setPos(pos.x(), pos.y(), pos.z());
        entity.setYRot(location.rotation().yaw());
        entity.setXRot(location.rotation().pitch());

        com.axiommc.fabric.entity.display.DisplayEntityUtil.applyItemDisplaySpec(entity, spec);
        level.addFreshEntity(entity);

        if (spec.ttl() > 0) {
            TaskScheduler.global().scheduleTask(spec.ttl(), () -> {
                if (!entity.isRemoved()) {
                    entity.discard();
                }
            });
        }

        return new com.axiommc.fabric.entity.display.FabricItemDisplayEntity(entity);
    }

    @Override
    public BlockDisplayEntity spawnBlockDisplay(BlockDisplaySpec spec, Location location) {
        net.minecraft.world.entity.Display.BlockDisplay entity =
            new net.minecraft.world.entity.Display.BlockDisplay(net.minecraft.world.entity.EntityType.BLOCK_DISPLAY, level);

        Vector3 pos = location.position();
        entity.setPos(pos.x(), pos.y(), pos.z());
        entity.setYRot(location.rotation().yaw());
        entity.setXRot(location.rotation().pitch());

        com.axiommc.fabric.entity.display.DisplayEntityUtil.applyBlockDisplaySpec(entity, spec);
        level.addFreshEntity(entity);

        if (spec.ttl() > 0) {
            TaskScheduler.global().scheduleTask(spec.ttl(), () -> {
                if (!entity.isRemoved()) {
                    entity.discard();
                }
            });
        }

        return new com.axiommc.fabric.entity.display.FabricBlockDisplayEntity(entity);
    }

    // ============================================================
    // Helper Methods
    // ============================================================

    private DimensionType mapDimensionType(net.minecraft.resources.ResourceKey<Level> dimension) {
        if (dimension == Level.OVERWORLD) {
            return DimensionType.OVERWORLD;
        } else if (dimension == Level.NETHER) {
            return DimensionType.NETHER;
        } else if (dimension == Level.END) {
            return DimensionType.THE_END;
        } else {
            return DimensionType.CUSTOM;
        }
    }
}
