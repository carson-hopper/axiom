#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Teleports a player to a target or coordinates.
 *
 * Usage: /tp <target> or /tp <target> <x> <y> <z>
 */
class TeleportCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "tp";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Teleport a player to a target or coordinates";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.player.teleport";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
