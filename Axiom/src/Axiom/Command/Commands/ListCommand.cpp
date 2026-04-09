#include "axpch.h"
#include "Axiom/Command/Commands/ListCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Application.h"

namespace Axiom {

    void ListCommand::Execute(CommandSender& sender, const std::vector<std::string>& /*arguments*/) {
        auto& server = Application::Instance().PacketCtx().Server();
        int count = server.PlayerCount();
        std::string names;
        for (const auto& player : server.AllPlayers()) {
            if (!names.empty()) names += ", ";
            names += player->Name();
        }
        std::string text = "There are " + std::to_string(count)
            + (count == 1 ? " player" : " players") + " online";
        if (!names.empty()) text += ": " + names;

        sender.SendMessage(ChatComponent::Create()
            .Text(text)
            .Color(ChatColor::White)
            .Build());
    }

    std::vector<std::string> ListCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {
        return {};
    }

}
