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
import com.axiommc.fabric.entity.display.DisplayEntityUtil;
import com.axiommc.fabric.entity.display.FabricBlockDisplayEntity;
import com.axiommc.fabric.entity.display.FabricItemDisplayEntity;
import com.axiommc.fabric.entity.display.FabricTextDisplayEntity;
import com.axiommc.fabric.particle.FabricParticleConverter;
import com.axiommc.fabric.player.FabricPlayer;
import com.axiommc.fabric.util.TaskScheduler;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Holder;
import net.minecraft.core.Registry;
import net.minecraft.core.particles.ParticleOptions;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.Identifier;
import net.minecraft.resources.ResourceKey;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.sounds.SoundSource;
import net.minecraft.world.entity.Display;
import net.minecraft.world.entity.EntityType;
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
        return getDimensionName(level.dimension());
    }

    private String getDimensionName(ResourceKey<Level> dimension) {
        if (dimension == Level.OVERWORLD) return "minecraft:overworld";
        if (dimension == Level.NETHER) return "minecraft:the_nether";
        if (dimension == Level.END) return "minecraft:the_end";
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
        var mcDimType = level.dimensionTypeRegistration().value();

        return new Dimension(
            name(),
            dimType,
            mcDimType.hasSkyLight(),
            mcDimType.hasCeiling(),
            mcDimType.hasFixedTime(),
            level.getMinY(),
            level.getHeight(),
            mcDimType.logicalHeight(),
            mcDimType.ambientLight(),
            mcDimType.coordinateScale()
        );
    }

    // ────────────────────────────────────────────────────────
    // Time Management
    // ────────────────────────────────────────────────────────

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

    // ────────────────────────────────────────────────────────
    // Weather
    // ────────────────────────────────────────────────────────

    @Override
    public Weather weather() {
        if (level.isThundering()) return Weather.THUNDER;
        if (level.isRaining()) return Weather.RAIN;
        return Weather.CLEAR;
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

    // ────────────────────────────────────────────────────────
    // Difficulty & PvP
    // ────────────────────────────────────────────────────────

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

    // ────────────────────────────────────────────────────────
    // Sound
    // ────────────────────────────────────────────────────────

    @Override
    public void playSound(SoundKey sound, float volume, float pitch, Vector3 position) {
        Registry<SoundEvent> registry = level().registryAccess().lookupOrThrow(Registries.SOUND_EVENT);

        Identifier identifier = Identifier.parse(sound.key());
        Optional<Holder.Reference<SoundEvent>> holder = registry.get(identifier);

        holder.ifPresent(soundEvent ->
                level().playSeededSound(
                        null,
                        position.x(),
                        position.y(),
                        position.z(),
                        soundEvent,
                        SoundSource.MASTER,
                        volume,
                        pitch,
                        level().getRandom().nextLong()
                )
        );
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

    // ────────────────────────────────────────────────────────
    // Chunk Loading
    // ────────────────────────────────────────────────────────

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

    // ────────────────────────────────────────────────────────
    // Block Access
    // ────────────────────────────────────────────────────────

    @Override
    public Block blockAt(int x, int y, int z) {
        int chunkX = toChunkCoord(x);
        int chunkZ = toChunkCoord(z);

        if (!chunkAt(chunkX, chunkZ).isPresent()) {
            loadChunk(chunkX, chunkZ);
        }

        return new FabricBlock(level, x, y, z, this);
    }

    @Override
    public int highestBlockY(int x, int z) {
        return level.getHeight(Heightmap.Types.WORLD_SURFACE, x, z);
    }

    // ============================================================
    // Particles & Display Entities
    // ============================================================

    // ────────────────────────────────────────────────────────
    // Particles
    // ────────────────────────────────────────────────────────

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

    // ────────────────────────────────────────────────────────
    // Display Entities
    // ────────────────────────────────────────────────────────

    @Override
    public TextDisplayEntity spawnTextDisplay(TextDisplaySpec spec, Location location) {
        Display.TextDisplay entity = new Display.TextDisplay(EntityType.TEXT_DISPLAY, level);
        setDisplayEntityPosition(entity, location);
        DisplayEntityUtil.applyTextDisplaySpec(entity, spec);
        level.addFreshEntity(entity);
        scheduleRemovalIfTtl(entity, spec.ttl());
        return new FabricTextDisplayEntity(entity);
    }

    @Override
    public ItemDisplayEntity spawnItemDisplay(ItemDisplaySpec spec, Location location) {
        Display.ItemDisplay entity = new Display.ItemDisplay(EntityType.ITEM_DISPLAY, level);
        setDisplayEntityPosition(entity, location);
        DisplayEntityUtil.applyItemDisplaySpec(entity, spec);
        level.addFreshEntity(entity);
        scheduleRemovalIfTtl(entity, spec.ttl());
        return new FabricItemDisplayEntity(entity);
    }

    @Override
    public BlockDisplayEntity spawnBlockDisplay(BlockDisplaySpec spec, Location location) {
        Display.BlockDisplay entity = new Display.BlockDisplay(EntityType.BLOCK_DISPLAY, level);
        setDisplayEntityPosition(entity, location);
        DisplayEntityUtil.applyBlockDisplaySpec(entity, spec);
        level.addFreshEntity(entity);
        scheduleRemovalIfTtl(entity, spec.ttl());
        return new FabricBlockDisplayEntity(entity);
    }

    // ════════════════════════════════════════════════════════
    // Helper Methods
    // ════════════════════════════════════════════════════════

    // ────────────────────────────────────────────────────────
    // Display Entity Helpers
    // ────────────────────────────────────────────────────────

    private void setDisplayEntityPosition(Display entity, Location location) {
        Vector3 pos = location.position();
        entity.setPos(pos.x(), pos.y(), pos.z());
        entity.setYRot(location.rotation().yaw());
        entity.setXRot(location.rotation().pitch());
    }

    private void scheduleRemovalIfTtl(Display entity, int ttl) {
        if (ttl <= 0) return;
        TaskScheduler.global().scheduleTask(ttl, () -> {
            if (!entity.isRemoved()) {
                entity.discard();
            }
        });
    }

    // ────────────────────────────────────────────────────────
    // Mapping Helpers
    // ────────────────────────────────────────────────────────

    private DimensionType mapDimensionType(ResourceKey<Level> dimension) {
        if (dimension == Level.OVERWORLD) return DimensionType.OVERWORLD;
        if (dimension == Level.NETHER) return DimensionType.NETHER;
        if (dimension == Level.END) return DimensionType.THE_END;
        return DimensionType.CUSTOM;
    }
}
