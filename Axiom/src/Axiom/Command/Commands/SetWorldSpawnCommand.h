#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Sets the world spawn point to the current position
 * or specific coordinates.
 *
 * Usage: /setworldspawn [x y z]
 * Permission: server.setworldspawn
 */
class SetWorldSpawnCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "setworldspawn";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set the world spawn point";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/setworldspawn [x y z]";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.world.setworldspawn";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
