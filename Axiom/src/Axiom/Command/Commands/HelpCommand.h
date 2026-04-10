#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

class CommandRegistry;

/**
 * Lists available commands and provides command help.
 *
 * Usage: /help [command]
 */
class HelpCommand : public Command {
public:
    explicit HelpCommand(CommandRegistry& registry);

    const std::string& Name() const override {
        static std::string name = "help";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "List available commands";
        return desc;
    }

    int RequiredPermissionLevel() const override { return 0; }

    Ref<LiteralNode> BuildTree() override;

private:
    CommandRegistry& m_Registry;
};

}
