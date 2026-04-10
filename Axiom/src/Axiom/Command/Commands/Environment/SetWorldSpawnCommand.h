#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Sets the world spawn point to the current position
 * or specific coordinates.
 *
 * Usage: /setworldspawn [<x> <y> <z>]
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

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.world.spawn.set";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
