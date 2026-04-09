#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Queries or changes the server difficulty level.
 *
 * Usage: /difficulty [peaceful|easy|normal|hard]
 * Permission: server.difficulty
 */
class DifficultyCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "difficulty";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Get or set the difficulty level";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/difficulty [level]";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.world.difficulty";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
