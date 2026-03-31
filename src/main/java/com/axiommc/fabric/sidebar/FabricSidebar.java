package com.axiommc.fabric.sidebar;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.player.Player;
import com.axiommc.api.sidebar.Sidebar;
import com.axiommc.fabric.chat.FabricComponentSerializer;
import com.axiommc.fabric.player.FabricPlayer;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.numbers.NumberFormat;
import net.minecraft.network.protocol.game.ClientboundResetScorePacket;
import net.minecraft.network.protocol.game.ClientboundSetDisplayObjectivePacket;
import net.minecraft.network.protocol.game.ClientboundSetObjectivePacket;
import net.minecraft.network.protocol.game.ClientboundSetScorePacket;
import net.minecraft.server.MinecraftServer;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.world.scores.DisplaySlot;
import net.minecraft.world.scores.Objective;
import net.minecraft.world.scores.Scoreboard;
import net.minecraft.world.scores.criteria.ObjectiveCriteria;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.CopyOnWriteArrayList;

public class FabricSidebar implements Sidebar {

    private static final String[] SCORE_HOLDERS = {
        "§0","§1","§2","§3","§4","§5","§6","§7","§8","§9",
        "§a","§b","§c","§d","§e"
    };

    private final String objectiveName;
    private Component title;
    private final Map<Integer, Component> lines = new HashMap<>();
    private final List<ServerPlayer> viewers = new CopyOnWriteArrayList<>();
    private final FabricComponentSerializer serializer = new FabricComponentSerializer();
    private Objective objective;
    private MinecraftServer server;

    public FabricSidebar(ChatComponent title, MinecraftServer server) {
        this.server = server;
        this.objectiveName = UUID.randomUUID().toString().replace("-", "").substring(0, 16);
        this.title = serializer.serialize(title);
        this.objective = createObjective();
    }

    private Objective createObjective() {
        Scoreboard scoreboard = server.getScoreboard();
        Objective obj = scoreboard.addObjective(
            objectiveName,
            ObjectiveCriteria.DUMMY,
            title,
            ObjectiveCriteria.RenderType.INTEGER,
            false,
            null
        );
        return obj;
    }

    @Override
    public void title(ChatComponent newTitle) {
        this.title = serializer.serialize(newTitle);
        objective.setDisplayName(title);
        for (ServerPlayer viewer : viewers) {
            viewer.connection.send(new ClientboundSetObjectivePacket(objective, 2));
        }
    }

    @Override
    public void line(int index, ChatComponent text) {
        if (index < 0 || index > 14) throw new IndexOutOfBoundsException("Line index must be 0-14");
        Component component = serializer.serialize(text);
        lines.put(index, component);
        for (ServerPlayer viewer : viewers) {
            sendLineToPlayer(viewer, index, component);
        }
    }

    @Override
    public void removeLine(int index) {
        if (index < 0 || index > 14) throw new IndexOutOfBoundsException("Line index must be 0-14");
        lines.remove(index);
        for (ServerPlayer viewer : viewers) {
            viewer.connection.send(new ClientboundResetScorePacket(SCORE_HOLDERS[index], objectiveName));
        }
    }

    @Override
    public void show(Player player) {
        ServerPlayer sp = ((FabricPlayer) player).player();
        viewers.add(sp);

        sp.connection.send(new ClientboundSetObjectivePacket(objective, 0));

        for (Map.Entry<Integer, Component> entry : lines.entrySet()) {
            sendLineToPlayer(sp, entry.getKey(), entry.getValue());
        }

        sp.connection.send(new ClientboundSetDisplayObjectivePacket(DisplaySlot.SIDEBAR, objective));
    }

    @Override
    public void hide(Player player) {
        ServerPlayer sp = ((FabricPlayer) player).player();
        viewers.remove(sp);
        sp.connection.send(new ClientboundSetObjectivePacket(objective, 1));
    }

    @Override
    public void destroy() {
        List<ServerPlayer> viewersCopy = new ArrayList<>(viewers);
        for (ServerPlayer viewer : viewersCopy) {
            hide(new FabricPlayer(viewer));
        }
        viewers.clear();
        lines.clear();
        server.getScoreboard().removeObjective(objective);
    }

    private void sendLineToPlayer(ServerPlayer player, int index, Component text) {
        int score = 15 - index;
        player.connection.send(new ClientboundSetScorePacket(
            SCORE_HOLDERS[index], objectiveName, score,
            Optional.empty(), Optional.empty()
        ));
    }
}
