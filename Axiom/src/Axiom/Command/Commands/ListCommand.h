#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Lists online players.
 *
 * Usage: /list
 * Permission: server.list
 */
class ListCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "list";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "List online players";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/list";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "server.list";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
