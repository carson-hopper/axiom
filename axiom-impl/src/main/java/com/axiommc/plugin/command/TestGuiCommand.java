package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.chat.Item;

import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Permission;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Subcommand;
import com.axiommc.api.gui.Gui;
import com.axiommc.api.gui.GuiItem;
import com.axiommc.api.gui.GuiSize;
import com.axiommc.api.player.Player;
import com.axiommc.api.sound.Sound;
import com.axiommc.fabric.Axiom;
import java.util.List;

@Command(name = "testgui")
@Description("Test GUI API")
@Permission("axiom.test")
public class TestGuiCommand {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /testgui <open|shop>").color(ChatColor.RED));
    }

    @Subcommand
    public void open(CommandSender sender) {
        sender.asPlayer().ifPresent(player -> {
            GuiItem border = GuiItem.of(Item.of("minecraft:gray_stained_glass_pane", " "));
            Gui gui = Gui.builder(GuiSize.TWO_ROWS)
                    .title("Test GUI")
                    .border(border)
                    .slot(4, GuiItem.of(
                            Item.of("minecraft:diamond", 1, "§bCenter Item", List.of("§7Click me!")),
                            event -> player.sendMessage(ChatComponent.text("Clicked center!").color(ChatColor.AQUA))
                    ))
                    .onClose(closedPlayer -> player.sendMessage(ChatComponent.text("GUI closed").color(ChatColor.YELLOW)))
                    .build();

            player.openGui(gui);
            player.sendMessage(ChatComponent.text("Opened GUI").color(ChatColor.GREEN));
        });
    }

    @Subcommand
    public void shop(CommandSender sender) {
        sender.asPlayer().ifPresent(player -> {
            GuiItem coinPouch = GuiItem.of(
                    Item.of("minecraft:gold_nugget", 1, "§6Coin Pouch",
                            List.of("§7Grants 500 coins", "§e Click to purchase")),
                    event -> {
                        event.player().clearTitle();
                        event.player().showTitle(
                                ChatComponent.text("+500 coins").color(ChatColor.color(0xFFD700)),
                                ChatComponent.text("Purchased!").color(ChatColor.WHITE),
                                5, 40, 10, 0
                        );
                        event.player().playSound(Sound.ORB_PICKUP, 1.0f, 1.0f);
                    }
            );

            GuiItem xpBoost = GuiItem.of(
                    Item.of("minecraft:experience_bottle", 1, "§aXP Boost",
                            List.of("§72x XP for 10 minutes", "§eCost: 1000 coins", "§e Click to purchase")),
                    event -> {
                        event.player().clearTitle();
                        event.player().showTitle(
                                ChatComponent.text("XP Boost Active!").color(ChatColor.color(0x55FF55)),
                                ChatComponent.text("2x XP for 10 minutes").color(ChatColor.WHITE),
                                5, 60, 15, 0
                        );
                        event.player().playSound(Sound.LEVEL_UP, 1.0f, 1.0f);
                    }
            );

            Gui shop = Gui.builder(GuiSize.THREE_ROWS)
                    .title("Economy Shop")
                    .slot(11, coinPouch)
                    .slot(13, xpBoost)
                    .slot(14, xpBoost)
                    .build();

            player.openGui(shop);
            player.sendMessage(ChatComponent.text("Opened GUI").color(ChatColor.GREEN));
        });
    }

}
