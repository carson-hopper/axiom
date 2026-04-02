package com.axiommc.plugin.command;

import com.axiommc.api.chat.ChatColor;
import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.command.CommandSender;
import com.axiommc.api.command.annotation.Command;
import com.axiommc.api.command.annotation.Description;
import com.axiommc.api.command.annotation.Execute;
import com.axiommc.api.command.annotation.Permission;

@Command(name = "axiom")
@Description("")
@Permission("axiom")
public class AxiomCommand {

    @Execute
    public void execute(CommandSender sender) {
        sender.sendMessage(ChatComponent.text("Usage: /axiom help").color(ChatColor.RED));
    }

    @Execute
    public void help(CommandSender sender) {
        sender.sendMessage("axiom");
    }
}
