#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Lists, queries, or sets game rules.
 *
 * Usage: /gamerule [rule] [value]
 * Permission: server.gamerule
 */
class GameruleCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "gamerule";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "List, query, or set game rules";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/gamerule [rule] [value]";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.world.gamerule";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
