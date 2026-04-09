#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Teleports a player to a target or coordinates.
 *
 * Usage: /teleport <target> [x y z]
 * Permission: server.teleport
 */
class TeleportCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "teleport";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Teleport a player to a target or coordinates";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/teleport <target> [x y z]";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.player.teleport";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
