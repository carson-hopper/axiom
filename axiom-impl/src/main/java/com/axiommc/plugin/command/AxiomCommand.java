package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.Command;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.CommandMeta;
import com.axiommc.api.command.annotation.Execute;

@CommandMeta(
        name = "axiom",
        aliases = {},
        description = "",
        permission = "axiom"
)
public class AxiomCommand implements Command {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /axiom help").color(ChatColor.RED));
    }

    @Execute
    public void help(CommandSender sender) {
        sender.sendMessage("axiom");
    }

}
