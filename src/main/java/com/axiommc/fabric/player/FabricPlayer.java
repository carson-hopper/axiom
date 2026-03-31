package com.axiommc.fabric.player;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.Sound;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.Axiom;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.entity.FabricLivingEntity;
import com.axiommc.fabric.mixin.net.minecraft.world.entity.player.PlayerAccessor;
import com.axiommc.fabric.util.TaskScheduler;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.core.Registry;
import net.minecraft.core.registries.Registries;
import net.minecraft.network.chat.Component;
import net.minecraft.network.protocol.game.ClientboundClearTitlesPacket;
import net.minecraft.network.protocol.game.ClientboundSetActionBarTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetSubtitleTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetTitleTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetTitlesAnimationPacket;
import net.minecraft.network.protocol.game.ClientboundSoundPacket;
import net.minecraft.resources.Identifier;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.sounds.SoundEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Optional;
import java.util.Set;

public class FabricPlayer extends FabricLivingEntity implements Player {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricPlayer.class);
    private final ServerPlayer player;

    public FabricPlayer(ServerPlayer player) {
        super(player);
        this.player = player;
    }

    public ServerPlayer player() {
        return player;
    }

    @Override
    public String nickname() {
        return player.getDisplayName().getString();
    }

    @Override
    public void nickname(String name) {
        player.setCustomName(Component.literal(name));
        player.setCustomNameVisible(true);
    }

    @Override
    public Location location() {
        var world = world();
        Vector3 position = new Vector3(player.getX(), player.getY(), player.getZ());
        Vector2 rotation = new Vector2(player.getYRot(), player.getXRot());
        return new Location(world, position, rotation);
    }

    @Override
    public void teleport(Location location) {
        Vector3 position = location.position();
        Vector2 rotation = location.rotation();

        if (location.world().name().equalsIgnoreCase(world().name())) {
            teleportSameWorld(position, rotation);
        } else {
            teleportDifferentWorld(location, position, rotation);
        }
    }

    private void teleportSameWorld(Vector3 position, Vector2 rotation) {
        player.teleportTo(position.x(), position.y(), position.z());
        rotation(rotation);
    }

    private void teleportDifferentWorld(Location location, Vector3 position, Vector2 rotation) {
        if (!(location.world() instanceof FabricWorld(net.minecraft.server.level.ServerLevel level))) {
            return;
        }
        player.teleportTo(level, position.x(), position.y(), position.z(), Set.of(), rotation.yaw(), rotation.pitch(), false);
    }

    @Override
    public World world() {
        return new FabricWorld(player.level());
    }

    @Override
    public boolean isOnline() {
        return !player.hasDisconnected();
    }

    @Override
    public void sendMessage(String message) {
        player.sendSystemMessage(Component.literal(message));
    }

    @Override
    public void sendMessage(ChatComponent message) {
        Component minecraftComponent = new FabricComponentSerializer().serialize(message);
        player.sendSystemMessage(minecraftComponent);
    }

    @Override
    public void kick(String reason) {
        player.connection.disconnect(Component.literal(reason));
    }

    @Override
    public void teleport(Server server, Location location) {
        // For cross-server teleportation, use TransitionPacket (MC 1.20.5+)
        try {
            var transitionPacketClass = Class.forName("net.minecraft.network.protocol.game.ClientboundTransitionPacket");
            var constructor = transitionPacketClass.getConstructor(String.class, int.class);
            var packet = constructor.newInstance(server.host(), server.port());
            player.connection.send((net.minecraft.network.protocol.Packet<?>) packet);
        } catch (Exception e) {
            LOGGER.error("Failed to transfer player {} to server {}:{}", name(), server.host(), server.port(), e);
        }
    }

    @Override
    public void transfer(Server server) {
        // TODO: Implement server transfer using Minecraft 1.20.5+ TransitionPacket
        // Example (Minecraft 1.20.5+):
        // try {
        //     Class<?> transitionPacketClass = Class.forName("net.minecraft.network.protocol.game.ClientboundTransitionPacket");
        //     Constructor<?> constructor = transitionPacketClass.getConstructor(String.class, int.class);
        //     Object packet = constructor.newInstance(server.host(), server.port());
        //     player.connection.send(packet);
        // } catch (Exception e) {
        //     logger.error("Failed to transfer player {}", name(), e);
        // }
        LOGGER.warn("Player transfer not yet implemented for {}", name());
    }

    @Override
    public double health() {
        return player.getHealth();
    }

    @Override
    public void health(double health) {
        player.setHealth((int) health);
    }

    @Override
    public double maxHealth() {
        return player.getMaxHealth();
    }

    @Override
    public void damage(double amount) {
        if (!(world() instanceof FabricWorld(net.minecraft.server.level.ServerLevel level))) {
            return;
        }
        playSound(Sound.PLAYER_DEATH, 1, 1);
        ((PlayerAccessor) player).invokeActuallyHurt(level, level.damageSources().generic(), (float) amount);
    }

    @Override
    public boolean hasPermission(String permission) {
        // For now, all players have all permissions
        // Permission system can be implemented later
        return true;
    }

    @Override
    public boolean isPlayer() {
        return true;
    }

    @Override
    public Optional<Player> asPlayer() {
        return Optional.of(this);
    }

    @Override
    public void showTitle(ChatComponent title, ChatComponent subtitle, int fadeIn, int stay, int fadeOut, int ttl) {
        var serializer = new FabricComponentSerializer();
        net.minecraft.network.chat.Component mcTitle = serializer.serialize(title);
        net.minecraft.network.chat.Component mcSubtitle = serializer.serialize(subtitle);
        player.connection.send(new ClientboundSetTitlesAnimationPacket(fadeIn, stay, fadeOut));
        player.connection.send(new ClientboundSetTitleTextPacket(mcTitle));
        player.connection.send(new ClientboundSetSubtitleTextPacket(mcSubtitle));
        if (ttl > 0) {
            TaskScheduler.global().scheduleTask(ttl, () ->
                player.connection.send(new ClientboundClearTitlesPacket(false))
            );
        }
    }

    @Override
    public void clearTitle() {
        player.connection.send(new ClientboundClearTitlesPacket(true));
    }

    @Override
    public void sendActionBar(ChatComponent component) {
        net.minecraft.network.chat.Component mc = new FabricComponentSerializer().serialize(component);
        player.connection.send(new ClientboundSetActionBarTextPacket(mc));
    }

    @Override
    public void playSound(SoundKey sound, float volume, float pitch) {
        Registry<SoundEvent> registry = player.level().registryAccess().lookupOrThrow(Registries.SOUND_EVENT);

        Identifier identifier = Identifier.parse(sound.key());
        registry.get(identifier).ifPresent(soundEvent -> {
            var packet = new ClientboundSoundPacket(
                soundEvent,
                player.getSoundSource(),
                location().position().x(),
                location().position().y(),
                location().position().z(),
                volume,
                pitch,
                player.level().getRandom().nextLong()
            );
            player.connection.send(packet);
        });
    }

    @Override
    public void openGui(Gui gui) {
        Axiom.guiManager().open(this, gui);
    }
}
