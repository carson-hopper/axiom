package com.axiommc.fabric.player;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.math.Vector2;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.player.Location;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.SoundKey;
import com.axiommc.api.world.Server;
import com.axiommc.api.world.World;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.util.TaskScheduler;
import com.axiommc.fabric.world.FabricWorld;
import net.minecraft.core.Holder;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.network.chat.Component;
import net.minecraft.network.protocol.game.ClientboundClearTitlesPacket;
import net.minecraft.network.protocol.game.ClientboundSetActionBarTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetSubtitleTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetTitleTextPacket;
import net.minecraft.network.protocol.game.ClientboundSetTitlesAnimationPacket;
import net.minecraft.network.protocol.game.ClientboundSoundPacket;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.sounds.SoundSource;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Optional;
import java.util.Set;
import java.util.UUID;

public record FabricPlayer(ServerPlayer player) implements Player {

    private static final Logger LOGGER = LoggerFactory.getLogger(FabricPlayer.class);

    @Override
    public UUID id() {
        return player.getUUID();
    }

    @Override
    public String name() {
        return player.getName().getString();
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
            player.teleportTo(position.x(), position.y(), position.z());
            rotation(location.rotation());
        } else if (location.world() instanceof FabricWorld(net.minecraft.server.level.ServerLevel level)) {
            player.teleportTo(level, position.x(), position.y(), position.z(), Set.of(), rotation.yaw(), rotation.pitch(), false);
        }
    }

    @Override
    public Vector3 velocity() {
        var motion = player.getDeltaMovement();
        return new Vector3(motion.x, motion.y, motion.z);
    }

    @Override
    public void velocity(Vector3 velocity) {
        player.setDeltaMovement(velocity.x(), velocity.y(), velocity.z());
    }

    @Override
    public Vector2 rotation() {
        return new Vector2(player.getYRot(), player.getXRot());
    }

    @Override
    public void rotation(Vector2 rotation) {
        player.setYRot(rotation.yaw());
        player.setXRot(rotation.pitch());
    }

    @Override
    public World world() {
        return new FabricWorld(player.level());
    }

    @Override
    public boolean alive() {
        return player.isAlive();
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
        if (world() instanceof FabricWorld(net.minecraft.server.level.ServerLevel level)) {
            player.hurt(level.damageSources().generic(), (float) amount);
        }
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
    public void sendActionBar(ChatComponent component) {
        net.minecraft.network.chat.Component mc = new FabricComponentSerializer().serialize(component);
        player.connection.send(new ClientboundSetActionBarTextPacket(mc));
    }

    @Override
    public void playSound(SoundKey sound, float volume, float pitch) {
        // Create a variable range sound event and send directly
        try {
            var idClass = Class.forName("net.minecraft.resources.Identifier");
            var constructor = idClass.getConstructor(String.class);
            var loc = constructor.newInstance(sound.key());

            @SuppressWarnings("unchecked")
            var castLoc = (net.minecraft.resources.Identifier) loc;
            var soundEvent = SoundEvent.createVariableRangeEvent(castLoc);
            var holder = net.minecraft.core.Holder.direct(soundEvent);

            player.connection.send(new ClientboundSoundPacket(
                holder,
                SoundSource.PLAYERS,
                player.getX(), player.getY(), player.getZ(),
                volume, pitch, player.getRandom().nextLong()
            ));
        } catch (Exception e) {
            LOGGER.error("Failed to play sound {} for player {}", sound.key(), name(), e);
        }
    }
}
