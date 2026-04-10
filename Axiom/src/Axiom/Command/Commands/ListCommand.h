#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Lists online players.
 *
 * Usage: /list
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

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.player.list";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 0; }

    Ref<LiteralNode> BuildTree() override;
};

}
