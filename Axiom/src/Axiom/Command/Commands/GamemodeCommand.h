#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Changes the game mode for a player.
 *
 * Usage: /gamemode <survival|creative|adventure|spectator>
 * Permission: server.gamemode
 */
class GamemodeCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "gamemode";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set the game mode";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/gamemode <mode>";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.player.gamemode";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
