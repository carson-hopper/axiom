package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Arg;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.math.Vector3;
import com.axiommc.api.world.Dimension;
import com.axiommc.api.world.World;
import com.axiommc.api.world.block.Block;
import com.axiommc.api.world.block.Material;
import com.axiommc.fabric.Axiom;

import java.util.Collection;
import java.util.Optional;

@CommandMeta(
        name = "testworld",
        description = "Test world and block APIs",
        permission = "axiom.test"
)
public class TestWorldCommand implements Command {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /testworld <worlds|info|block|setblock|highest>").color(ChatColor.RED));
    }

    @Subcommand
    public void worlds(CommandSender sender) {
        Collection<World> worlds = Axiom.worlds();
        if (worlds.isEmpty()) {
            sender.sendMessage(ChatComponent.text("No worlds loaded").color(ChatColor.RED));
            return;
        }

        sender.sendMessage(ChatComponent.text("Loaded worlds:").color(ChatColor.GOLD));
        for (World w : worlds) {
            sender.sendMessage(ChatComponent.text("  - " + w.name() + " (" + w.players().size() + " players)")
                    .color(ChatColor.WHITE));
        }
    }

    @Subcommand
    public void info(CommandSender sender) {
        Optional<World> world = Axiom.world("minecraft:overworld");
        if (world.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Overworld not found").color(ChatColor.RED));
            return;
        }

        World w = world.get();
        sender.sendMessage(ChatComponent.text("World: " + w.name()).color(ChatColor.GOLD));
        sender.sendMessage(ChatComponent.text("Players: " + w.players().size()).color(ChatColor.WHITE));

        Dimension dim = w.dimension();
        sender.sendMessage(ChatComponent.text("Dimension Properties:").color(ChatColor.YELLOW));
        sender.sendMessage(ChatComponent.text("  Min Y: " + dim.minY()).color(ChatColor.GRAY));
        sender.sendMessage(ChatComponent.text("  Height: " + dim.height()).color(ChatColor.GRAY));
    }

    @Subcommand
    public void block(CommandSender sender, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z) {
        Optional<World> world = Axiom.world("minecraft:overworld");
        if (world.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Overworld not found").color(ChatColor.RED));
            return;
        }

        Block block = world.get().blockAt(x, y, z);
        sender.sendMessage(ChatComponent.text("Block at " + x + "," + y + "," + z + ": " + block.type().id())
                .color(ChatColor.GREEN));
    }

    @Subcommand
    public void setblock(CommandSender sender, @Arg("x") int x, @Arg("y") int y, @Arg("z") int z, @Arg("material") String material) {
        Optional<World> world = Axiom.world("minecraft:overworld");
        if (world.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Overworld not found").color(ChatColor.RED));
            return;
        }

        world.get().blockAt(x, y, z).type(Material.of(material));
        sender.sendMessage(ChatComponent.text("Set block to " + material).color(ChatColor.GREEN));
    }

    @Subcommand
    public void highest(CommandSender sender, @Arg("x") int x, @Arg("z") int z) {
        Optional<World> world = Axiom.world("minecraft:overworld");
        if (world.isEmpty()) {
            sender.sendMessage(ChatComponent.text("Overworld not found").color(ChatColor.RED));
            return;
        }

        int y = world.get().highestBlockY(x, z);
        sender.sendMessage(ChatComponent.text("Highest block at " + x + "," + z + ": Y=" + y)
                .color(ChatColor.GREEN));
    }

}
